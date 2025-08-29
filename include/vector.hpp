#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <cstddef>

template <typename T, typename Allocator = std::allocator<T>>
class Vector
{   
    template <bool IsConst>
    class Iterator;

    using value_type             = T;
    using allocator_type         = Allocator;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using pointer                = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer          = typename std::allocator_traits<Allocator>::const_pointer;
    using iterator               = Iterator<false>;
    using const_iterator         = Iterator<true>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
/***********************************
      Special Member Functions 
***********************************/
    constexpr Vector() { }
    ~Vector() { clear(); }

    explicit Vector(size_type n)
        : size_(n),
          capacity_(n)
    {
        data_ = std::allocator_traits<allocator_type>::allocate(alloc_, capacity_);
        std::uninitialized_default_construct_n(data_, size_);
    }

    Vector(const Vector& other)
    {
        size_ = other.size_;
        capacity_ = other.capacity_;

        data_ = std::allocator_traits<allocator_type>::allocate(alloc_, capacity_);

        for (int i{}; i<size_; ++i)
            data_[i] = other.data_[i];
    }

    Vector& operator=(const Vector& other)
    {
        if (this == &other)
            return *this;

        std::destroy(data_, data_+size_);
        std::allocator_traits<allocator_type>::deallocate(alloc_, data_, capacity_);

        size_ = other.size_;
        capacity_ = other.capacity_;

        data_ = std::allocator_traits<allocator_type>::allocate(alloc_, capacity_);

        for (int i{}; i<size_; ++i)
            data_[i] = other.data_[i];

        return *this;
    }

    constexpr Vector(Vector&& other) noexcept
        : alloc_(std::move(other.alloc_)),
          data_(other.data_),
          size_(other.size_),
          capacity_(other.capacity_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    constexpr Vector& operator=(Vector&& other) noexcept
    {
        if (this == &other)
            return *this;

        alloc_ = std::move(other.alloc_);

        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;

        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;

        return *this;
    }

    void assign(size_type count, const value_type& val)
    {
        std::destroy(data_, data_+size_);
        size_ = count;
        if (count > capacity_)
        {
            std::allocator_traits<allocator_type>::deallocate(alloc_, data_, capacity_);
            data_ = std::allocator_traits<allocator_type>::allocate(alloc_, count);
            capacity_ = count;
        }
        std::uninitialized_fill_n(data_, count, val);
    }
    [[nodiscard]] allocator_type get_allocator() const noexcept { return alloc_; }

/***********************************
          Element Access 
***********************************/
    [[nodiscard]] constexpr reference at(size_type idx)
    {
        if (idx < 0 || idx >= size_) throw std::out_of_range("Index out of range.");
        return data_[idx];
    }

    [[nodiscard]] constexpr const_reference at(size_type idx) const
    {
        if (idx < 0 || idx >= size_) throw std::out_of_range("Index out of range.");
        return data_[idx];
    }

    [[nodiscard]] constexpr reference operator[](size_type idx) { return data_[idx]; }
    [[nodiscard]] constexpr const_reference operator[](size_type idx) const { return data_[idx]; }

    [[nodiscard]] constexpr reference front() { return data_[0]; }
    [[nodiscard]] constexpr const_reference front() const { return data_[0]; }

    [[nodiscard]] constexpr reference back() { return data_[size_-1]; }
    [[nodiscard]] constexpr const_reference back() const { return data_[size_-1]; }

    [[nodiscard]] constexpr pointer data() { return data_; }
    [[nodiscard]] constexpr const_pointer data() const { return data_; }

/***********************************
             Iterators
***********************************/
    [[nodiscard]] constexpr iterator begin() noexcept { return iterator{data_}; }
    [[nodiscard]] constexpr const_iterator begin() const noexcept { return const_iterator{data_}; }

    [[nodiscard]] constexpr iterator end() noexcept { return iterator{data_+size_}; }
    [[nodiscard]] constexpr const_iterator end() const noexcept { return const_iterator{data_+size_}; }

    [[nodiscard]] constexpr reverse_iterator rbegin() noexcept { return reverse_iterator{end()}; }
    [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator{end()}; }
    
    [[nodiscard]] constexpr reverse_iterator rend() noexcept { return reverse_iterator{begin()}; }
    [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator{begin()}; }
    
    [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return begin(); }
    [[nodiscard]] constexpr const_iterator cend() const noexcept { return end(); }

    [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return rend(); }


/***********************************
             Capacity 
***********************************/
    [[nodiscard]] constexpr size_type size() const noexcept { return size_; } 
    [[nodiscard]] constexpr size_type capacity() const noexcept { return capacity_; } 
    [[nodiscard]] constexpr bool empty() const noexcept { return size_ == 0; } 

    constexpr void reserve(size_type new_capacity_)
    {
        if (new_capacity_ > capacity_)
            resize(new_capacity_);
    }

    constexpr void shrink_to_fit() { resize(size_); } 


/***********************************
             Modifiers 
***********************************/

    constexpr void clear() noexcept
    {
        std::destroy(data_, data_+size_);
        std::allocator_traits<allocator_type>::deallocate(alloc_, data_, capacity_);

        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
    }

    // TODO: implement range support for insert and erase
    // supporting functions are implemented
    template<typename U>
    constexpr iterator insert(const_iterator pos, U&& val)
    { 
        difference_type idx = pos-cbegin();
        shift_data_(idx, 1);
        data_[idx] = std::forward<U>(val);
        return iterator{data_+idx};
    }

    constexpr iterator erase(const_iterator pos)
    {
        difference_type idx = pos-cbegin();


    }


    template <typename U>
    constexpr void push_back(U&& val) noexcept
    {
        grow_if_full_();
        data_[size_] = std::forward<U>(val);
        ++size_;
    }

    constexpr void pop_back() noexcept
    {
        --size_;
        std::destroy_at(data_+size_);
    }

    constexpr void swap(Vector& other)
    {
        using std::swap;
        swap(alloc_, other.alloc_);
        swap(data_, other.data_);
        swap(size_, other.size_);
        swap(capacity_, other.capacity_);
    }

    template<typename U>
    constexpr void insert(size_type idx, U&& val)
    {
        grow_if_full_();
        for (size_type i{size_}; i > idx; --i)
            data_[i] = std::move(data_[i-1]); 

        data_[idx] = std::forward<U>(val);
        ++size_;
    }

    constexpr void resize(size_type new_capacity_)
    {
        if (new_capacity_ < size_)
        {
            std::destroy(data_+new_capacity_, data_+size_);
            size_ = new_capacity_;
        }

        pointer new_data_ = std::allocator_traits<allocator_type>::allocate(alloc_, new_capacity_);
        for (int i{}; i<size_; ++i)
            new_data_[i] = std::move(data_[i]);

        std::allocator_traits<allocator_type>::deallocate(alloc_, data_, capacity_);

        capacity_ = new_capacity_;
        data_ = new_data_;
    }

private: 
    [[no_unique_address]] allocator_type alloc_{};
    pointer   data_{ nullptr };
    size_type size_{};
    size_type capacity_{};

    constexpr void grow_if_full_() { if (size_ == capacity_) { resize(capacity_*2); }  }
    constexpr void shift_data_(size_t start, int shift)
    {
        size_t new_size_ = size_ + shift;
        if (new_size_ > capacity_) 
            resize(new_size_*2);

        size_t offset = shift < 0 ? -shift : shift;
        if (shift > 0)
        {
            for (int i{size_+offset-1}; i>start+offset; --i)
                data_[i] = std::move(data_[i-offset]);
        }
        else
        {
            std::destroy(data_+start, data_+start+offset);
            for (size_t i{start}; i<start+offset; ++i)
                data_[i] = std::move(data_[i+offset]);
            // should we destruct unspecified state moved from elements?
        }

        size_ = new_size_;
    }
};

template <typename T, typename Allocator>
template<bool IsConst>
class Vector<T, Allocator>::Iterator
{
public:
    using pointer   = std::conditional_t<IsConst, Vector::const_pointer, Vector::pointer>;
    using reference = std::conditional_t<IsConst, Vector::const_pointer, Vector::pointer>;



};