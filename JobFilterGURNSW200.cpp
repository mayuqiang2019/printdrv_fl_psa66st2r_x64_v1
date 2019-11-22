#include "stdafx.h"
#include "filter.h"


/// <summary>Checks if transformation is needed for specified job.</summary>
/// <param name="job">Job to be checked.</param>
/// <returns>True if transformation is needed, false otherwise.</returns>
bool CJobFilterGURNSW200::NeedTransform(const print::CJob& job)
{
	switch(job.m_nsTemplateID)
	{
	case 0x4E	:
		return true;
	} // switch...

	return CJobFilterGUR126003::NeedTransform(job);
}

/// <summary>Performs necessary transformation on the job.</summary>
/// <param name="job">Job to be transformed.</param>
/// <returns>Transformed job.</returns>
print::CJob CJobFilterGURNSW200::Transform(const print::CJob& job)
{
	switch(job.m_nsTemplateID)
	{
	case 0x4E	:
		return TransformTempl_N(job);
	} // switch...

	return CJobFilterGUR126003::Transform(job);
}

/// <summary>Transforms job for template N.</summary>
/// <param name="job">Job to be transformed.</param>
/// <returns>Transformed job.</returns>
print::CJob CJobFilterGURNSW200::TransformTempl_N(const print::CJob& job)
{
	if(job.GetCount() != 21)	{ return job; }

	print::CJob newJob(job);
	POS pos(newJob.FindIndex(7));
	// firmware version
	if(!newJob.GetAt(pos).m_strData.GetLength())
	{
		newJob.GetAt(pos).m_strData = L"GURNSW200";
	}

	return newJob;
}