#include <algorithm>        // for copy, fill, max, min, move
#include <cstddef>          // for size_t, ptrdiff_t
#include <iterator>         // for reverse_iterator, random_access_iterator_tag
#include <memory>           // for swap, allocator_traits, allocator
#include <memory_resource>  // for allocator_traits
#include <stdexcept>        // for out_of_range
#include <type_traits>      // for conditional_t, enable_if_t
#include <utility>          // for swap

template<typename Allocator = std::allocator<char>>
class BoolVector {
    std::size_t sz_usage_ = 0; // if 1 block max - 8 usage
    std::size_t sz_allocated_blocks_ = 0; //if 8 usage - 1 block
    char* data_ = nullptr;

    Allocator allocator_;
    using AllocTraits = std::allocator_traits<Allocator>;
private:
    class BoolObject {
        int indx_;
        char* ptr_;
    public:
        BoolObject(char* ptr, int indx) : indx_(indx), ptr_(ptr) {} 
        
        BoolObject& operator=(bool value) {
            if (value) {
                *ptr_ |= (1 << indx_);
            } else {
                *ptr_ &= ~(1 << indx_);
            }
            return *this;
        }

        BoolObject& operator=(const BoolObject& other) {
            return *this = static_cast<bool>(other);
        }

        operator bool() const {
            return (*ptr_ >> indx_) & 1;
        }
    };

    template <bool is_const>
    class IteratorImpl {
        using PtrType = std::conditional_t<is_const, const char*, char*>;
        PtrType data_ = nullptr;
        int cur_indx = 0;
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = bool;
        using pointer = std::conditional_t<is_const, bool, BoolObject>;
        using reference = std::conditional_t<is_const, bool, BoolObject>;
        using iterator_category = std::random_access_iterator_tag;

        IteratorImpl() = default;
        explicit IteratorImpl(PtrType ptr, int indx = 0) : data_(ptr), cur_indx(indx) {}
        
        template <bool was_const = is_const, typename = std::enable_if_t<was_const>>
        IteratorImpl(const IteratorImpl<false>& other) : data_(other.data_), cur_indx(other.cur_indx) {}

        reference operator*() const {
            if constexpr (is_const) {
                return (*data_ >> cur_indx) & 1;
            } else {
                return BoolObject(data_, cur_indx);
            }
        }
        
        pointer operator->() const {
            return **this;
        }
        
        reference operator[](difference_type n) const {
            return *( *this + n );
        }

        IteratorImpl& operator++() {
            ++cur_indx;
            if (cur_indx > 7) {
                cur_indx = 0;
                ++data_;
            } 
            return *this;
        }
        
        IteratorImpl operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
        
        IteratorImpl& operator--() {
            --cur_indx;
            if (cur_indx < 0) {
                cur_indx = 7;
                --data_;
            }
            return *this;
        }
        
        IteratorImpl operator--(int) {
            auto tmp = *this;
            --*this;
            return tmp;
        }
        
        IteratorImpl operator+(difference_type n) const {
            IteratorImpl result = *this;
            result += n;
            return result;
        }
        
        IteratorImpl operator-(difference_type n) const {
            IteratorImpl result = *this;
            result -= n;
            return result;
        }
        
        IteratorImpl& operator+=(difference_type n) {
            difference_type total_bits = cur_indx + n;
            if (total_bits >= 0) {
                data_ += total_bits / 8;
                cur_indx = total_bits % 8;
            } else {
                difference_type byte_offset = (-total_bits + 7) / 8;
                data_ -= byte_offset;
                cur_indx = total_bits + byte_offset * 8;
            }
            return *this;
        }
        
        IteratorImpl& operator-=(difference_type n) {
            return *this += (-n);
        }
        
        difference_type operator-(const IteratorImpl& other) const {
            return (data_ - other.data_) * 8 + (cur_indx - other.cur_indx);
        }
        
        bool operator==(const IteratorImpl& other) const {
            return data_ == other.data_ && cur_indx == other.cur_indx;
        }
        
        bool operator!=(const IteratorImpl& other) const {
            return !(*this == other);
        }
        
        bool operator<(const IteratorImpl& other) const {
            if (data_ == other.data_) return cur_indx < other.cur_indx;
            return data_ < other.data_;
        }
        
        bool operator>(const IteratorImpl& other) const {
            return other < *this;
        }
        
        bool operator<=(const IteratorImpl& other) const {
            return !(other < *this);
        }
        
        bool operator>=(const IteratorImpl& other) const {
            return !(*this < other);
        }

        friend class IteratorImpl<true>;
    };
public:
    using value_type = bool;
    using allocator_type = Allocator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = BoolObject;
    using const_reference = bool;
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
    
    using iterator = IteratorImpl<false>;
    using const_iterator = IteratorImpl<true>;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
public:
    BoolVector() {}
    
    explicit BoolVector(const Allocator& alloc) : allocator_(alloc) {}

    ~BoolVector() {
        if (data_) {
            AllocTraits::deallocate(allocator_, data_, sz_allocated_blocks_);
        }
    }

    explicit BoolVector(size_type size) {
        if (size > 0) {
            size_type byte_size = (size + 7) / 8;
            data_ = AllocTraits::allocate(allocator_, byte_size);
            sz_allocated_blocks_ = byte_size;
            sz_usage_ = size;
            std::fill(data_, data_ + byte_size, 0);
        }
    }

    BoolVector(const BoolVector& other) : allocator_(AllocTraits::select_on_container_copy_construction(other.allocator_)) {
        if (other.size() > 0) {
            data_ = AllocTraits::allocate(allocator_, other.sz_allocated_blocks_);
            sz_allocated_blocks_ = other.sz_allocated_blocks_;
            sz_usage_ = other.sz_usage_;
            std::copy(other.data_, other.data_ + sz_allocated_blocks_, data_);
        }
    }

    BoolVector(BoolVector&& other) noexcept 
        : sz_usage_(other.sz_usage_), sz_allocated_blocks_(other.sz_allocated_blocks_), data_(other.data_), allocator_(std::move(other.allocator_)) {
        other.sz_usage_ = 0;
        other.sz_allocated_blocks_ = 0;
        other.data_ = nullptr;
    }

    BoolVector& operator=(BoolVector&& other) noexcept {
        swap(other);
        return *this;
    }

public:
    iterator begin() {
        return iterator(data_, 0);
    }
    const_iterator begin() const {
        return const_iterator(data_, 0);
    }
    iterator end() {
        return iterator(data_ + size() / 8, size() % 8);
    }
    const_iterator end() const {
        return const_iterator(data_ + size() / 8, size() % 8);
    }

    const_iterator cbegin() const {
        return begin();
    }
    const_iterator cend() const {
        return end();
    }

    reference operator[](size_type pos) {
        return begin()[pos];
    }
    const_reference operator[](size_type pos) const {
        return begin()[pos];
    }

    reference at(size_type pos) {
        if (pos >= size()) throw std::out_of_range("BoolVector::at");
        return operator[](pos);
    }
    const_reference at(size_type pos) const {
        if (pos >= size()) throw std::out_of_range("BoolVector::at");
        return operator[](pos);
    }

    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }
    reference back() { return *(end() - 1); }
    const_reference back() const { return *(end() - 1); }

    bool operator==(const BoolVector& other) const {
        if (size() != other.size()) return false;
        if (size() == 0) return true;

        size_type byte_size = size() / 8;
        for (size_type i = 0; i < byte_size; ++i) {
            if (data_[i] != other.data_[i])
                return false;
        }

        size_type remaining_bits = size() % 8;
        if (remaining_bits > 0) {
            char mask = (1 << remaining_bits) - 1;
            if ((data_[byte_size] & mask) != (other.data_[byte_size] & mask))
                return false;
        }

        return true;
    }
    bool operator!=(const BoolVector& other) const {
        return !(*this == other);
    }

    void swap(BoolVector& other) noexcept {
        std::swap(sz_usage_, other.sz_usage_);
        std::swap(sz_allocated_blocks_, other.sz_allocated_blocks_);
        std::swap(data_, other.data_);
        if constexpr (std::allocator_traits<Allocator>::propagate_on_container_swap::value) {
            std::swap(allocator_, other.allocator_);
        }
    }

    size_type size() const noexcept {
        return sz_usage_;
    }
    
    size_type capacity() const noexcept {
        return sz_allocated_blocks_ * 8;
    }

    size_type max_size() const noexcept {
        return AllocTraits::max_size(allocator_) * 8;
    }

    bool empty() const noexcept {
        return sz_usage_ == 0;
    }

    void clear() noexcept {
        sz_usage_ = 0;
    }

    void reserve(size_type new_cap) {
        size_type new_blocks = (new_cap + 7) / 8;
        if (new_blocks > sz_allocated_blocks_) {
            reallocate(new_blocks);
        }
    }

    void shrink_to_fit() {
        size_type required_blocks = (sz_usage_ + 7) / 8;
        if (required_blocks < sz_allocated_blocks_) {
            reallocate(required_blocks);
        }
    }

    allocator_type get_allocator() const noexcept {
        return allocator_;
    }

    void push_back(bool value) {
        if (sz_usage_ / 8 >= sz_allocated_blocks_) {
           reallocate(std::max<size_type>(1, sz_allocated_blocks_ * 2));
        }
        *end() = value;
        ++sz_usage_;
    }

    void pop_back() {
        --sz_usage_;
        *end() = false;
    }

    void resize(size_type count, bool value = false) {
        if (count > size()) {
            reserve(count);
            size_type old_size = size();
            sz_usage_ = count;
            for (size_type i = old_size; i < count; ++i) {
                (*this)[i] = value;
            }
        } else if (count < size()) {
            while (size() > count) {
                pop_back();
            }
        }
    }

    BoolVector& operator=(const BoolVector& other) {
        if (this != &other) {
            BoolVector temp(other);
            swap(temp);
        }
        return *this;
    }
private:
    void reallocate(size_type new_capacity_blocks) {
        if (new_capacity_blocks == sz_allocated_blocks_) return;

        char* new_data_ = nullptr;
        if (new_capacity_blocks > 0) {
            new_data_ = AllocTraits::allocate(allocator_, new_capacity_blocks);
            size_type blocks_to_copy = std::min(sz_allocated_blocks_, new_capacity_blocks);
            if (data_ && blocks_to_copy > 0) {
                std::copy(data_, data_ + blocks_to_copy, new_data_);
            }
            if (new_capacity_blocks > sz_allocated_blocks_) {
                std::fill(new_data_ + sz_allocated_blocks_, new_data_ + new_capacity_blocks, 0);
            }
        }
        
        if (data_) {
            AllocTraits::deallocate(allocator_, data_, sz_allocated_blocks_);
        }
        
        data_ = new_data_;
        sz_allocated_blocks_ = new_capacity_blocks;
    }
};