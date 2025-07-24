#include <stdexcept>
#include <utility>
#include <cstddef>

template <typename T>
class Vector
{   public:
        Vector()
        {
           vec = new T[capacity_]; 
        }

        ~Vector()
        {
            delete[] vec;
        }

        Vector& operator=(const Vector& other)
        {
            if (this == &other)
                return *this;

            size_ = other.size_;
            capacity_ = other.capacity_;

            delete[] vec;
            vec = new T[capacity_];

            for (int i{}; i<size_; ++i)
            {
                vec[i] = other.vec[i];
            }

            return *this;
        }

        T& operator[](size_t idx)
        {
            return vec[idx];
        }

        T& at(size_t idx)
        {
            if (idx < 0 || idx >= size_)
                throw std::out_of_range("Index out of range.");
            return vec[idx];
        }

        T& front()
        {
            return vec[0];
        }

        T& back()
        {
            return vec[size_-1];
        }

        Vector<T>* data()
        {
            return this;
        }


        void push_back(T& value)
        {
            if (size_ == capacity_)
                resize(capacity_*2);

            vec[size_] = value;
            ++size_;
        }

        void push_back(T&& value)
        {
            if (size_ == capacity_)
                resize(capacity_*2);

            vec[size_] = std::move(value);
            ++size_;
        }

        void pop_back() noexcept
        {
            if (size_)
                --size_;

            if((float)size_ / capacity_ < decrease_size_bound_)
                resize((capacity_+1)/2);
        }

        void swap(size_t idx1, size_t idx2)
        {
            T temp = std::move(vec[idx2]);
            vec[idx2] = std::move(vec[idx1]);
            vec[idx1] = std::move(temp);
        }

        void insert(size_t idx, T&& val)
        {
            if (size_+1 >= capacity_)
                resize(capacity_*2);
            for (size_t i{size_}; i > idx; --i)
                vec[i] = std::move(vec[i-1]); 

            vec[idx] = val;
            
            ++size_;
        }

        size_t size() const noexcept { return size_; } 
        size_t capacity() const noexcept { return capacity_; } 
        bool empty() const noexcept { return size_ == 0; } 
        void clear() noexcept
        {
            delete[] vec;
            size_ = 0;
            capacity_ = 8;
            vec = new T[capacity_];
        }

        void reserve(size_t new_capacity_)
        {
            if (new_capacity_ > capacity_)
            {
                resize(new_capacity_);
            }
        }

        void shrink_to_fit()
        {
            resize(size_);
        }

        void resize(size_t new_capacity_)
        {
            capacity_ = new_capacity_;
            size_ = std::min(size_, capacity_);
            T* new_vec = new T[capacity_];

            for (int i{}; i<size_; ++i)
            {
                new_vec[i] = std::move(vec[i]);
            }

            delete vec;
            vec = new_vec;
        }

    private: 
        T* vec{ nullptr };
        size_t size_{ };
        size_t capacity_{ 8 };
        //static constexpr float increase_size_bound_{ 0.8 };
        static constexpr float decrease_size_bound_{ 0.2 };
};