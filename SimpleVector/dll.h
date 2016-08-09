#ifndef _DLL_H_
#define _DLL_H_

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>

#define _CAPACITY_RATE_ 1.1

template<class T>
class __SimpleVector {
private:
	//value list
	T * m_list;
	//posize_t list of value
	T** m_sortPointList;
	//
	size_t m_maxSize;
	//
	size_t m_size;

	bool relocate(size_t extendToSize) {
		T* newData = new T[extendToSize];
		if (newData == NULL) {
			return false;
		}
		T** newDataPosize_t = new T*[extendToSize];
		if (newDataPosize_t == NULL) {
			delete[] newData;
			return false;
		}
		if(m_size >0 && m_list != NULL){
			for(size_t j = 0; j < m_size; j++){
			   newData[j]=m_list[j];
			}
		}
		if(extendToSize - m_size > 0){
			//memset(newData + m_size, 0, sizeof(T) * (extendToSize - m_size));
		}
		for (size_t j = 0; j < extendToSize; j++) {
			newDataPosize_t[j] = newData + j;
		}
		if (m_sortPointList != NULL)
			delete[] m_sortPointList;
		if (m_list != NULL)
			delete[] m_list;
		m_list = newData;
		m_sortPointList = newDataPosize_t;
		m_maxSize = extendToSize;
		return true;
	}

	bool copy(const __SimpleVector & par){
		if (m_maxSize < par.m_size) {
			if (!relocate(par.m_size)) {
				return false;
			}
		} else {
			for (size_t i = 0; i < m_size; i++) {
				m_list[i].~T();
			}
		}

		for (size_t i = 0; i < par.m_size; i++) {
			m_list[i] = *(par.m_sortPointList[i]);
			m_sortPointList[i] = &m_list[i];
		}
		m_size = par.m_size;
		//memset(m_list + m_size, 0, sizeof(T) * (m_maxSize - m_size));
		return true;
	}
	T** getSortPointList() {
		return m_sortPointList;
	}

public:
	static int compare_int(const void* arg1, const void* arg2) {
		int** a = (int**) arg1;
		int** b = (int**) arg2;
		return (**a) - (**b);
	}

	static int compare_long(const void* arg1, const void* arg2) {
		long** a = (long**) arg1;
		long** b = (long**) arg2;
		if((**a) == (**b)) {
			return 0;
		}
		if((**a) < (**b)) {
			return -1;
		}

		return 1;
	}

	static int compare_SimpleVector(const void* arg1, const void* arg2) {
		T** a = (T**) arg1;
		T** b = (T**) arg2;
		return (**a).compare(**b);
	}

	static int compare_Attr_SimpleVector(const void* arg1, const void* arg2) {
		T** a = (T**) arg1;
		T** b = (T**) arg2;
		return (**a).compareAttr(**b);
	}



	__SimpleVector() {
		m_maxSize = 0;
		m_size = 0;
		m_list = NULL;
		m_sortPointList = NULL;
	}

	bool init(size_t initSize) {
		//forbid relocate
		if(initSize == 0){
			return true;
		}
       if(m_size != 0 || m_list !=0){
			throw -1;
			return false;
       }

	    m_list = new T[initSize];
		if (m_list == NULL) {
			return false;
		}
		 m_sortPointList = new T*[initSize];
		if (m_sortPointList == NULL) {
			delete[] m_sortPointList;
			return false;
		}
		m_maxSize = initSize;


		for (size_t i = 0; i < initSize; i++) {
			m_sortPointList[i] = m_list + i;
			m_size++;
		}
		return true;
	}


	__SimpleVector & operator=(const __SimpleVector & par) {
		copy(par);
		return *this;
	}

	__SimpleVector(const __SimpleVector & par) {
		m_maxSize = 0;
		m_size = 0;
		m_list = NULL;
		m_sortPointList = NULL;
		copy(par);   //forbid =,Recursion
	}

	explicit __SimpleVector(size_t initSize) {
		m_maxSize = 0;
		m_size = 0;
		m_list = NULL;
		m_sortPointList = NULL;
		relocate(initSize);
	}

	~__SimpleVector() {
		if (m_list != NULL) {
			delete[] m_list;
			m_list = NULL;
		}
		if (m_sortPointList != NULL) {
			delete[] m_sortPointList;
			m_sortPointList = NULL;
		}
	}
	void sort() {
		if (m_size <= 1)
			return;
		qsort(m_sortPointList, (size_t) m_size, (size_t) sizeof(T*), compare_SimpleVector);
	}
	void sortAttr() {
		if (m_size <= 1)
			return;
		qsort(m_sortPointList, (size_t) m_size, (size_t) sizeof(T*), compare_Attr_SimpleVector);
	}

	void sort_by_int() {
		if (m_size <= 1)
			return;
		qsort(m_sortPointList, (size_t) m_size, (size_t) sizeof(T*), compare_int);
	}
	void sort_by_long() {
		if (m_size <= 1)
			return;
		qsort(m_sortPointList, (size_t) m_size, (size_t) sizeof(T*), compare_long);
	}

	void sort_by_fn(int (*cmp)(const void*, const void*)) {
		if (m_size <= 1)
			return;
		qsort(m_sortPointList, (size_t) m_size, (size_t) sizeof(T*), cmp);
	}

	bool push_back(const T &t){
		return push(t);
	}

	bool push(const T &t) {
		if (m_size == m_maxSize) {
			size_t newsize =
					((size_t) (m_maxSize * _CAPACITY_RATE_) + 1) > m_size + 1 ?
							((size_t) (m_maxSize * _CAPACITY_RATE_) + 1) : m_size + 1;
			if (!relocate(newsize)) {
				return false;
			}
		}
		*(m_list + m_size) = t;
		m_sortPointList[m_size] = (m_list + m_size);
		m_size++;
		return true;
	}

	bool pushArray(T* t, size_t length) {
		if (m_size + length > m_maxSize) {
			size_t newsize =
					((size_t) (m_maxSize * _CAPACITY_RATE_) + 1) > m_size + length ?
							((size_t) (m_maxSize * _CAPACITY_RATE_) + 1) : m_size + length;
			if (!relocate(newsize)) {
				return false;
			}
		}
		for (size_t i = 0; i < length; i++) {
			*(m_list + m_size + i) = *(t + i);
			m_sortPointList[m_size + i] = (m_list + m_size + i);
		}
		m_size = m_size + length;
		return true;
	}

	T * find(const T &x) {
		if (m_size <= 0) {
			return NULL;
		}
		const T* posize_t;
		posize_t = &x;
		T** search_result = (T**) bsearch(&posize_t, m_sortPointList, (size_t) m_size,
				(size_t) sizeof(T*), compare_SimpleVector);
		if (search_result == NULL) {
			return NULL;
		}
		return *search_result;
	}

	T * findAttr(const T &x) {
		if (m_size <= 0) {
			return NULL;
		}
		const T* posize_t;
		posize_t = &x;
		T** search_result = (T**) bsearch(&posize_t, m_sortPointList, (size_t) m_size,
				(size_t) sizeof(T*), compare_Attr_SimpleVector);
		if (search_result == NULL) {
			return NULL;
		}
		return *search_result;
	}

	T * find_by_int(const T &x) {
		if (m_size <= 0) {
			return NULL;
		}
		const T* posize_t;
		posize_t = &x;
		T** search_result = (T**) bsearch(&posize_t, m_sortPointList, (size_t) m_size,
				(size_t) sizeof(T*), compare_int);
		if (search_result == NULL) {
			return NULL;
		}
		return *search_result;
	}
	T * find_by_long(const T &x) {
		if (m_size <= 0) {
			return NULL;
		}
		const T* posize_t;
		posize_t = &x;
		T** search_result = (T**) bsearch(&posize_t, m_sortPointList, (size_t) m_size,
				(size_t) sizeof(T*), compare_long);
		if (search_result == NULL) {
			return NULL;
		}
		return *search_result;
	}

	T * find_by_fn(const T &x, int (*cmp)(const void*, const void*)) {
		if (m_size <= 0) {
			return NULL;
		}
		const T* posize_t;
		posize_t = &x;
		T** search_result = (T**) bsearch(&posize_t, m_sortPointList, (size_t) m_size,
				(size_t) sizeof(T*), cmp);
		if (search_result == NULL) {
			return NULL;
		}
		return *search_result;
	}

	int findPostion(const T &x) {
		if (m_size <= 0) {
			return -1;
		}
		const T* posize_t;
		posize_t = &x;
		T** position = (T**) bsearch(&posize_t, m_sortPointList, (size_t) m_size,
				(size_t) sizeof(T*), compare_SimpleVector);
		if (position == NULL) {
			return -1;
		}
		return position - m_sortPointList;
	}

	int findAttrPostion(const T &x) {
		if (m_size <= 0) {
			return -1;
		}
		const T* posize_t;
		posize_t = &x;

		T** position = (T**) bsearch(&posize_t, m_sortPointList, (size_t) m_size,
				(size_t) sizeof(T*), compare_Attr_SimpleVector);
		if (position == NULL) {
			return -1;
		}
		return position - m_sortPointList;
	}

	int findPostion_by_int(const T &x) {
		if (m_size <= 0) {
			return -1;
		}
		const T* posize_t;
		posize_t = &x;
		T** position = (T**) bsearch(&posize_t, m_sortPointList, (size_t) m_size,
				(size_t) sizeof(T*), compare_int);
		if (position == NULL) {
			return -1;
		}
		return position - m_sortPointList;
	}
	int findPostion_by_long(const T &x) {
		if (m_size <= 0) {
			return -1;
		}
		const T* posize_t;
		posize_t = &x;
		T** position = (T**) bsearch(&posize_t, m_sortPointList, (size_t) m_size,
				(size_t) sizeof(T*), compare_long);
		if (position == NULL) {
			return -1;
		}
		return position - m_sortPointList;
	}

	inline T * const at(const size_t index) {
		if (index > m_size - 1 || index < 0 || m_size==0) {
			return NULL;
		}
		return m_sortPointList[index];
	}

	inline size_t size() const{
		return m_size;
	}
	inline size_t capacity() {
		return m_maxSize;
	}

	bool reserve(size_t toSize) {
		if (m_maxSize >= toSize) {
			return true;
		}
		return relocate(toSize);
	}

	void merge(const __SimpleVector<T> & v) {
		if (v.m_size > 0) {
			pushArray(v.m_list, v.m_size);
		}
	}

	inline T & operator[](const size_t index) {
		if (index > m_size - 1 || index < 0 || m_size==0) {
			throw -1;
		}
		return *(m_sortPointList[index]);
	}
	inline const T & operator[](const size_t index) const{
		if (index > m_size - 1 || index < 0 || m_size==0 ) {;
			throw -1;
		}
		return *(m_sortPointList[index]);
	}

	inline void setCount(const size_t count) {
		if (count > m_size) {
			throw -1;
		}
		m_size = count;
		return;
	}

	int findLowest(const T &x, int * lowest) {
		int finded_count = 0;
		int fined_pos = -1;
		*lowest = -1;
		if (m_size <= 0) {
			return finded_count;
		}
		const T* p_x = &x;
		T** find_item = (T**) bsearch(&p_x, m_sortPointList, (size_t) m_size, (size_t) sizeof(T*),
				compare_SimpleVector);
		if (find_item == NULL) {
			return finded_count;
		}
		finded_count = 1; //self count
		fined_pos = find_item - m_sortPointList;
		*lowest = fined_pos;
		while (*lowest > 0) {
			if (compare_SimpleVector(&(m_sortPointList[*lowest - 1]), &p_x) == 0) {
				*lowest = *lowest - 1;
				finded_count = finded_count + 1;
			} else {
				break;
			}
		}
		while (fined_pos < m_size - 1) {
			if (compare_SimpleVector(&(m_sortPointList[fined_pos + 1]), &p_x) == 0) {
				fined_pos = fined_pos + 1;
				finded_count = finded_count + 1;
			} else {
				break;
			}
		}
		return finded_count;
	}

	int findAttrLowest(const T &x, int * lowest) {
		int finded_count = 0;
		int fined_pos = -1;
		*lowest = -1;
		if (m_size <= 0) {
			return finded_count;
		}
		const T* p_x = &x;
		T** find_item = (T**) bsearch(&p_x, m_sortPointList, (size_t) m_size, (size_t) sizeof(T*),
				compare_Attr_SimpleVector);
		if (find_item == NULL) {
			return finded_count;
		}
		finded_count = 1; //self count
		fined_pos = find_item - m_sortPointList;
		*lowest = fined_pos;
		while (*lowest > 0) {
			if (compare_Attr_SimpleVector(&(m_sortPointList[*lowest - 1]), &p_x) == 0) {
				*lowest = *lowest - 1;
				finded_count = finded_count + 1;
			} else {
				break;
			}
		}
		while (fined_pos < m_size - 1) {
			if (compare_Attr_SimpleVector(&(m_sortPointList[fined_pos + 1]), &p_x) == 0) {
				fined_pos = fined_pos + 1;
				finded_count = finded_count + 1;
			} else {
				break;
			}
		}
		return finded_count;
	}
};
#endif /* _DLL_H_ */
