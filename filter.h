#pragma once

#include "printPrinter.h"

/// <summary>Performs necessary transformation on the job before sending
/// them to printer.</summary>
class IJobFilter
{
public:
	//static const wchar_t* const s_strNeverExpires;

    /// <summary>Destructor.</summary>
	virtual ~IJobFilter() {}

    /// <summary>Checks if transformation is needed for specified job.</summary>
    /// <param name="job">Job to be checked.</param>
    /// <returns>True if transformation is needed, false otherwise.</returns>
	virtual bool NeedTransform(const print::CJob& job) = 0;

    /// <summary>Performs necessary transformation on the job.</summary>
    /// <param name="job">Job to be transformed.</param>
    /// <returns>Transformed job.</returns>
    virtual print::CJob Transform(const print::CJob& job) = 0;
};

/// <summary>Job filter for firmware GUR126003.</summary>
class CJobFilterGUR126003 : public IJobFilter
{
public:
    virtual bool NeedTransform(const print::CJob& job);
    virtual print::CJob Transform(const print::CJob& job);

protected:
	print::CJob TransformTempl_0(const print::CJob& job);
	print::CJob TransformTempl_1(const print::CJob& job);
	print::CJob TransformTempl_2(const print::CJob& job);
	print::CJob TransformTempl_3(const print::CJob& job);
	print::CJob TransformTempl_4(const print::CJob& job);
	print::CJob TransformTempl_5(const print::CJob& job);
	print::CJob TransformTempl_6(const print::CJob& job);
	print::CJob TransformTempl_7(const print::CJob& job);
	print::CJob TransformTempl_8(const print::CJob& job);
	print::CJob TransformTempl_9(const print::CJob& job);
	print::CJob TransformTempl_A(const print::CJob& job);
	print::CJob TransformTempl_B(const print::CJob& job);

  void TransformExpiry(print::CData& data)
  {
    if(!data.m_strData.CompareNoCase(L"9999 days") ||
      !data.m_strData.CompareNoCase(L"9999days") ||
      !data.m_strData.CompareNoCase(L"9999 day") ||
      !data.m_strData.CompareNoCase(L"9999day"))
    {
		data.m_strData = _T("Ticket Never expires");
    }
  }
};

/// <summary>Job filter for firmware GUR126001.</summary>
class CJobFilterGUR126001 : public CJobFilterGUR126003 {};

/// <summary>Job filter for firmware GUR109000.</summary>
class CJobFilterGUR109000 : public CJobFilterGUR126003 {};

/// <summary>Job filter for firmware GUREUR100.</summary>
class CJobFilterGUREUR100 : public CJobFilterGUR126003 {};

/// <summary>Job filter for firmware GUREUR101.</summary>
class CJobFilterGUREUR101 : public CJobFilterGUR126003 {};

/// <summary>Job filter for firmware GUREUR102.</summary>
class CJobFilterGUREUR102 : public CJobFilterGUR126003 {};

/// <summary>Job filter for firmware GUREUR200.</summary>
class CJobFilterGUREUR200 : public CJobFilterGUR126003 {};

/// <summary>Job filter for firmware GURTOR100.</summary>
class CJobFilterGURTOR100 : public CJobFilterGUR126003 {};

/// <summary>Job filter for firmware GURTOR101.</summary>
class CJobFilterGURTOR101 : public CJobFilterGUR126003 {};

/// <summary>Job filter for firmware GURUSA001.</summary>
class CJobFilterGURUSA001 : public CJobFilterGUR126003 {};

/// <summary>Job filter for firmware GURUSA003.</summary>
class CJobFilterGURUSA003 : public CJobFilterGUR126003 {};

/// <summary>Job filter for firmware GURNSW200.</summary>
class CJobFilterGURNSW200 : public CJobFilterGUR126003
{
public:
    virtual bool NeedTransform(const print::CJob& job);
    virtual print::CJob Transform(const print::CJob& job);

protected:
	print::CJob TransformTempl_N(const print::CJob& job);
};


/// <summary>Job filter for firmware GURSNG103.</summary>
class CJobFilterGURSNG103 : public CJobFilterGUR126003 {};

