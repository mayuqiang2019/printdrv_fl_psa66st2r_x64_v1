#include "stdafx.h"
#include "filter.h"

/// <summary>Checks if transformation is needed for specified job.</summary>
/// <param name="job">Job to be checked.</param>
/// <returns>True if transformation is needed, false otherwise.</returns>
bool CJobFilterGUR126003::NeedTransform(const print::CJob& job)
{
	switch(job.m_nsTemplateID)
	{
	case 0x00	:
	case 0x01	:
	case 0x02	:
	case 0x03	:
	case 0x04	:
	case 0x05	:
	case 0x06	:
	case 0x07	:
	case 0x08	:
	case 0x09	:
	case 0x0A	:
	case 0x0B	:
		POS pos = job.FindIndex(job.GetCount() - 3);
		//TransformExpiry(job.GetAt(pos));
		return true;
	} // switch...

	return false;
}

/// <summary>Performs necessary transformation on the job.</summary>
/// <param name="job">Job to be transformed.</param>
/// <returns>Transformed job.</returns>
print::CJob CJobFilterGUR126003::Transform(const print::CJob& job)
{
	switch(job.m_nsTemplateID)
	{
	case 0x00	:
		return TransformTempl_0(job);
	case 0x01	:
		return TransformTempl_1(job);
	case 0x02	:
		return TransformTempl_2(job);
	case 0x03	:
		return TransformTempl_3(job);
	case 0x04	:
		return TransformTempl_4(job);
	case 0x05	:
		return TransformTempl_5(job);
	case 0x06	:
		return TransformTempl_6(job);
	case 0x07	:
		return TransformTempl_7(job);
	case 0x08	:
		return TransformTempl_8(job);
	case 0x09	:
		return TransformTempl_9(job);
	case 0x0A	:
		return TransformTempl_A(job);
	case 0x0B	:
		return TransformTempl_B(job);
	} // switch...

	return job;
}

/// <summary>Transforms job for template 0.</summary>
/// <param name="job">Job to be transformed.</param>
/// <returns>Transformed job.</returns>
print::CJob CJobFilterGUR126003::TransformTempl_0(const print::CJob& job)
{
	if(job.GetCount() != 17) { return job; }

	print::CJob newJob(job);
	POS pos(newJob.FindIndex(11));

	if(!newJob.GetAt(pos).m_strData.GetLength())
	{
		newJob.GetAt(pos).m_strData = L" ";
	}

  pos = newJob.FindIndex(14);
  TransformExpiry(newJob.GetAt(pos));

	return newJob;
}

/// <summary>Transforms job for template 1.</summary>
/// <param name="job">Job to be transformed.</param>
/// <returns>Transformed job.</returns>
print::CJob CJobFilterGUR126003::TransformTempl_1(const print::CJob& job)
{
	if(job.GetCount() != 15) { return job; }

	print::CJob newJob;
	print::CData data;
	POS pos(job.GetHeadPos());

	newJob.m_nsTemplateID = job.m_nsTemplateID;

	newJob.AddTail(job.GetNext(pos)); // i
	newJob.AddTail(job.GetNext(pos)); // 1
	newJob.AddTail(job.GetNext(pos)); // 2
	newJob.AddTail(job.GetNext(pos)); // 3
	newJob.AddTail(job.GetNext(pos)); // 5
	newJob.AddTail(print::CData()); // A
	newJob.AddTail(job.GetNext(pos)); // B
	newJob.AddTail(job.GetNext(pos)); // C
	newJob.AddTail(job.GetNext(pos)); // D
	newJob.AddTail(job.GetNext(pos)); // E
	newJob.AddTail(job.GetNext(pos)); // F

	data = job.GetNext(pos);
	if(!data.m_strData.GetLength()) { data.m_strData = L" "; }
	newJob.AddTail(data); // G

	newJob.AddTail(job.GetNext(pos)); // I
	newJob.AddTail(print::CData()); // J
	
  POS posExpiry = newJob.AddTail(job.GetNext(pos)); // K
  TransformExpiry(newJob.GetAt(posExpiry));

	newJob.AddTail(job.GetNext(pos)); // L
	newJob.AddTail(job.GetNext(pos)); // Z

	return newJob;
}

/// <summary>Transforms job for template 2.</summary>
/// <param name="job">Job to be transformed.</param>
/// <returns>Transformed job.</returns>
print::CJob CJobFilterGUR126003::TransformTempl_2(const print::CJob& job)
{
	if(job.GetCount() != 17) { return job; }

	print::CJob newJob(job);
	POS pos(newJob.FindIndex(11));

	if(!newJob.GetAt(pos).m_strData.GetLength())
	{
		newJob.GetAt(pos).m_strData = L" ";
	}

  pos = newJob.FindIndex(14);
  TransformExpiry(newJob.GetAt(pos));

	return newJob;
}

/// <summary>Transforms job for template 3.</summary>
/// <param name="job">Job to be transformed.</param>
/// <returns>Transformed job.</returns>
print::CJob CJobFilterGUR126003::TransformTempl_3(const print::CJob& job)
{
	if(job.GetCount() != 15) { return job; }

	print::CJob newJob;
	print::CData data;
	POS pos(job.GetHeadPos());

	newJob.m_nsTemplateID = job.m_nsTemplateID;

	newJob.AddTail(job.GetNext(pos)); // 1
	newJob.AddTail(job.GetNext(pos)); // 2
	newJob.AddTail(job.GetNext(pos)); // 3
	newJob.AddTail(print::CData()); // 7
	newJob.AddTail(job.GetNext(pos)); // N
	newJob.AddTail(job.GetNext(pos)); // O

	data = job.GetNext(pos);
	if(!data.m_strData.GetLength()) { data.m_strData = L" "; }
	newJob.AddTail(data); // P

	newJob.AddTail(job.GetNext(pos)); // Q
	newJob.AddTail(job.GetNext(pos)); // R
	newJob.AddTail(job.GetNext(pos)); // S
	newJob.AddTail(job.GetNext(pos)); // T
	newJob.AddTail(job.GetNext(pos)); // U
	newJob.AddTail(job.GetNext(pos)); // J
	
  POS posExpiry = newJob.AddTail(job.GetNext(pos)); // K
  TransformExpiry(newJob.GetAt(posExpiry));

	newJob.AddTail(job.GetNext(pos)); // L
	newJob.AddTail(job.GetNext(pos)); // X

	return newJob;
}

/// <summary>Transforms job for template 4.</summary>
/// <param name="job">Job to be transformed.</param>
/// <returns>Transformed job.</returns>
print::CJob CJobFilterGUR126003::TransformTempl_4(const print::CJob& job)
{
	if(job.GetCount() != 17) { return job; }

	print::CJob newJob(job);
	POS pos(newJob.FindIndex(7));

	if(!newJob.GetAt(pos).m_strData.GetLength())
	{
		newJob.GetAt(pos).m_strData = L" ";
	}

  pos = newJob.FindIndex(14);
  TransformExpiry(newJob.GetAt(pos));

	return newJob;
}

/// <summary>Transforms job for template 5.</summary>
/// <param name="job">Job to be transformed.</param>
/// <returns>Transformed job.</returns>
print::CJob CJobFilterGUR126003::TransformTempl_5(const print::CJob& job)
{
	if(job.GetCount() != 17) { return job; }

	print::CJob newJob(job);
	POS pos(newJob.FindIndex(7));

	if(!newJob.GetAt(pos).m_strData.GetLength())
	{
		newJob.GetAt(pos).m_strData = L" ";
	}

  pos = newJob.FindIndex(14);
  TransformExpiry(newJob.GetAt(pos));

	return newJob;
}

/// <summary>Transforms job for template 6.</summary>
/// <param name="job">Job to be transformed.</param>
/// <returns>Transformed job.</returns>
print::CJob CJobFilterGUR126003::TransformTempl_6(const print::CJob& job)
{
	if(job.GetCount() != 3) { return job; }

	print::CJob newJob(job);
	print::CData data;

	newJob.AddTail(data); // d
	newJob.AddTail(data); // e
	newJob.AddTail(data); // f
	newJob.AddTail(data); // g

	return newJob;
}

/// <summary>Transforms job for template 7.</summary>
/// <param name="job">Job to be transformed.</param>
/// <returns>Transformed job.</returns>
print::CJob CJobFilterGUR126003::TransformTempl_7(const print::CJob& job)
{
	if(job.GetCount() != 16) { return job; }

	print::CJob newJob;
	print::CData data;
	POS pos(job.GetHeadPos());

	newJob.m_nsTemplateID = job.m_nsTemplateID;

	newJob.AddTail(job.GetNext(pos)); // i
	newJob.AddTail(job.GetNext(pos)); // 1
	newJob.AddTail(job.GetNext(pos)); // 2
	newJob.AddTail(job.GetNext(pos)); // 3
	newJob.AddTail(job.GetNext(pos)); // h
	newJob.AddTail(print::CData()); // A
	newJob.AddTail(job.GetNext(pos)); // B
	newJob.AddTail(job.GetNext(pos)); // C
	newJob.AddTail(job.GetNext(pos)); // D
	newJob.AddTail(job.GetNext(pos)); // E
	newJob.AddTail(job.GetNext(pos)); // F

	data = job.GetNext(pos);
	if(!data.m_strData.GetLength()) { data.m_strData = L" "; }
	newJob.AddTail(data); // G

	newJob.AddTail(job.GetNext(pos)); // I
	newJob.AddTail(job.GetNext(pos)); // J
	
  POS posExpiry = newJob.AddTail(job.GetNext(pos)); // K
  TransformExpiry(newJob.GetAt(posExpiry));

	newJob.AddTail(job.GetNext(pos)); // L
	newJob.AddTail(job.GetNext(pos)); // Z

	return newJob;
}

/// <summary>Transforms job for template 8.</summary>
/// <param name="job">Job to be transformed.</param>
/// <returns>Transformed job.</returns>
print::CJob CJobFilterGUR126003::TransformTempl_8(const print::CJob& job)
{
	if(job.GetCount() != 17) { return job; }

	print::CJob newJob(job);
	POS pos(newJob.FindIndex(7));

	if(!newJob.GetAt(pos).m_strData.GetLength())
	{
		newJob.GetAt(pos).m_strData = L" ";
	}

  pos = newJob.FindIndex(14);
  TransformExpiry(newJob.GetAt(pos));

	return newJob;
}

/// <summary>Transforms job for template 9.</summary>
/// <param name="job">Job to be transformed.</param>
/// <returns>Transformed job.</returns>
print::CJob CJobFilterGUR126003::TransformTempl_9(const print::CJob& job)
{
	if(job.GetCount() != 16) { return job; }

	print::CJob newJob;
	print::CData data;
	POS pos(job.GetHeadPos());

	newJob.m_nsTemplateID = job.m_nsTemplateID;

	newJob.AddTail(job.GetNext(pos)); // i
	newJob.AddTail(job.GetNext(pos)); // 1
	newJob.AddTail(job.GetNext(pos)); // 2
	newJob.AddTail(job.GetNext(pos)); // 3
	newJob.AddTail(job.GetNext(pos)); // 4
	newJob.AddTail(job.GetNext(pos)); // A
	newJob.AddTail(job.GetNext(pos)); // B
	newJob.AddTail(job.GetNext(pos)); // C
	newJob.AddTail(job.GetNext(pos)); // D
	newJob.AddTail(job.GetNext(pos)); // E
	newJob.AddTail(job.GetNext(pos)); // F

	data = job.GetNext(pos);
	if(!data.m_strData.GetLength()) { data.m_strData = L" "; }
	newJob.AddTail(data); // G

	newJob.AddTail(job.GetNext(pos)); // I
	
  POS posExpiry = newJob.AddTail(job.GetNext(pos)); // K
  TransformExpiry(newJob.GetAt(posExpiry));
  CWkString strTmp = newJob.GetAt(posExpiry).m_strData;
  newJob.GetAt(posExpiry).m_strData = L"Ticket Void After: " + strTmp;

	newJob.AddTail(job.GetNext(pos)); // L
	newJob.AddTail(job.GetNext(pos)); // Z

	return newJob;
}

/// <summary>Transforms job for template A.</summary>
/// <param name="job">Job to be transformed.</param>
/// <returns>Transformed job.</returns>
print::CJob CJobFilterGUR126003::TransformTempl_A(const print::CJob& job)
{
	if(job.GetCount() != 16) { return job; }

	print::CJob newJob;
	print::CData data;
	POS pos(job.GetHeadPos());

	newJob.m_nsTemplateID = job.m_nsTemplateID;

	newJob.AddTail(job.GetNext(pos)); // i
	newJob.AddTail(job.GetNext(pos)); // 1
	newJob.AddTail(job.GetNext(pos)); // 2
	newJob.AddTail(job.GetNext(pos)); // 3
	newJob.AddTail(job.GetNext(pos)); // h
	newJob.AddTail(job.GetNext(pos)); // N
	newJob.AddTail(job.GetNext(pos)); // O

	data = job.GetNext(pos);
	if(!data.m_strData.GetLength()) { data.m_strData = L" "; }
	newJob.AddTail(data); // P

	newJob.AddTail(job.GetNext(pos)); // Q
	newJob.AddTail(job.GetNext(pos)); // R
	newJob.AddTail(job.GetNext(pos)); // S
	newJob.AddTail(job.GetNext(pos)); // T
	newJob.AddTail(job.GetNext(pos)); // U

	POS posExpiry = newJob.AddTail(job.GetNext(pos)); // K
  TransformExpiry(newJob.GetAt(posExpiry));

	newJob.AddTail(job.GetNext(pos)); // L
	newJob.AddTail(job.GetNext(pos)); // Z

	return newJob;
}

/// <summary>Transforms job for template B.</summary>
/// <param name="job">Job to be transformed.</param>
/// <returns>Transformed job.</returns>
print::CJob CJobFilterGUR126003::TransformTempl_B(const print::CJob& job)
{
	if(job.GetCount() != 3) { return job; }

	print::CJob newJob(job);
	print::CData data;
	newJob.AddTail(data); // d
	newJob.AddTail(data); // e
	newJob.AddTail(data); // f
	newJob.AddTail(data); // g

	return newJob;
}