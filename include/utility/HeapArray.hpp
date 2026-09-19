/***************************************************************************************************
 * @file  HeapArray.hpp
 * @brief Declaration of the HeapArray class
 **************************************************************************************************/

#pragma once

#include <initializer_list>

/**
 * @class HeapArray
 * @brief A minimal dynamic array class that manually manages heap memory for a sequence of `Type`.
 * @tparam Type The type of the array's data.
 */
template <typename Type>
class HeapArray {
public:
    /**
     * @brief Default constructor. Does not allocate any data.
     */
    HeapArray() : size(0), data(nullptr) { }

    /**
     * @brief Constructs an array with a given size. Elements are default-initialized (Type()).
     * @param size Number of elements to allocate.
     */
    explicit HeapArray(size_t size)
        : size(size), data(new Type[size]) {
        Type default_value = Type();
        for(unsigned int i = 0 ; i < size ; ++i) {
            data[i] = default_value;
        }
    }

    /**
     * @brief Constructs an array with a given size, filling all elements with a specified value.
     * @param size Number of elements.
     * @param default_value Value to fill the array with.
     */
    HeapArray(size_t size, Type default_value)
        : size(size), data(new Type[size]) {
        for(unsigned int i = 0 ; i < size ; ++i) {
            data[i] = default_value;
        }
    }

    /**
     * @brief Constructs an array from an initializer_list.
     */
    HeapArray(const std::initializer_list<Type>& values)
        : size(values.size()), data(new Type[size]) {
        for(unsigned int i = 0 ; const Type& value : values) { data[i++] = value; }
    }

    /**
     * @brief Copy constructor. Performs a deep copy.
     */
    HeapArray(const HeapArray& other)
        : size(other.size), data(new Type[size]) {
        for(unsigned int i = 0 ; i < size ; ++i) { data[i] = other[i]; }
    }

    /**
     * @brief Destructor. Releases allocated memory.
     */
    ~HeapArray() {
        delete[] data;
    }

    /**
     * @brief Copy assignment operator. Performs a deep copy.
     */
    HeapArray& operator=(const HeapArray& other) {
        if(this == &other) { return *this; }

        delete[] data;

        size = other.size;
        data = new Type[size];
        for(unsigned int i = 0 ; i < size ; ++i) { data[i] = other[i]; }

        return *this;
    }

    /**
     * @brief Access element at `index`.
     * @note No bounds checking.
     */
    Type& operator[](size_t index) { return data[index]; }

    /**
     * @brief Access element at `index`.
     * @note No bounds checking.
     */
    const Type& operator[](size_t index) const { return data[index]; }

    /**
     * @return Current number of elements in the array.
     */
    size_t get_size() const { return size; }

    /**
     * @return Raw pointer to owned data.
     */
    Type* get_data() const { return data; }

    /**
     * @brief Resizes the array. If expanded, new elements are default-constructed. If shrunk,
     * extra elements are discarded.
     * @param new_size New size of the array.
     */
    void resize(size_t new_size) {
        if(new_size > 0) {
            Type* temp = data;
            data = new Type[new_size];
            Type default_value = Type();
            for(unsigned int i = 0 ; i < new_size ; ++i) { data[i] = i < size ? temp[i] : default_value; }
            size = new_size;
            delete[] temp;
        } else {
            delete[] data;
            size = 0;
            data = nullptr;
        }
    }

    /**
     * @return True if the array has no elements.
     */
    bool empty() const { return size == 0; }

    /**
     * @return Iterator to the beginning of the array.
     */
    Type* begin() { return data; }

    /**
     * @return Const iterator to the beginning of the array.
     */
    const Type* begin() const { return data; }

    /**
     * @return Iterator to one past the end of the array.
     */
    Type* end() { return data + size; }

    /**
     * @return Const iterator to one past the end of the array.
     */
    const Type* end() const { return data + size; }

private:
    size_t size; ///< Number of elements in the array.
    Type* data;  ///< Pointer to heap-allocated data.
};
