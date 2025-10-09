#ifndef COM_PTR_H
#define COM_PTR_H

template <typename PtrType>
class ComPtr
{
public:
	ComPtr()
		: m_ptr(nullptr)
	{

	}

	explicit ComPtr(PtrType* ptr)
		: m_ptr(ptr)
	{

	}

	ComPtr(const ComPtr& rhs)
		: m_ptr(nullptr)
	{
		CopyFrom(rhs);
	}

	ComPtr(ComPtr&& rhs)
		: m_ptr(nullptr)
	{
		MoveFrom(std::move(rhs));
	}

	~ComPtr()
	{
		Reset();
	}

	operator PtrType* ()
	{
		return m_ptr;
	}

	operator PtrType* () const
	{
		return m_ptr;
	}

	ComPtr& operator =(const ComPtr& rhs)
	{
		if (&rhs != this)
		{
			Reset();
			CopyFrom(rhs);
		}
		return (*this);
	}

	ComPtr& operator =(ComPtr&& rhs)
	{
		Reset();
		MoveFrom(std::move(rhs));
		return (*this);
	}

	PtrType* operator ->() const
	{
		return m_ptr;
	}

	PtrType** operator &()
	{
		return &m_ptr;
	}

	bool operator ==(const ComPtr& rhs) const
	{
		return m_ptr == rhs.m_ptr;
	}

	PtrType* Detach()
	{
		PtrType* result = m_ptr;
		m_ptr = nullptr;
		return result;
	}

	void Reset()
	{
		if (m_ptr != nullptr)
		{
			m_ptr->Release();
			m_ptr = nullptr;
		}
	}

	bool IsEmpty() const
	{
		return (m_ptr == nullptr);
	}

private:
	void CopyFrom(const ComPtr& src)
	{
		assert(m_ptr == nullptr);
		m_ptr = src.m_ptr;
		if (m_ptr != nullptr)
		{
			m_ptr->AddRef();
		}
	}

	void MoveFrom(ComPtr&& src)
	{
		std::swap(m_ptr, src.m_ptr);
	}

	PtrType* m_ptr;
};

#endif
