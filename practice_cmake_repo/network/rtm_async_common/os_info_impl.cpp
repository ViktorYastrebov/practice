#include "os_info_impl.h"

//#include <boost/archive/binary_iarchive.hpp>
//#include <boost/archive/binary_oarchive.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

os_info_impl::os_info_impl()
  : base_task_id_impl(base_task_impl::OS_INFO)
{}

os_info_impl::~os_info_impl()
{}

void os_info_impl::execute() {
  description_ = get_os_description();
}

const std::string  &os_info_impl::get_description() const {
  return description_;
}

std::string  &os_info_impl::get_description() {
  return description_;
}

#ifdef _WIN32
#include <windows.h>
#include <lm.h>
#elif defined __linux__
#include <stdexcept>

//getting last error
#include <errno.h>
#include <string.h>
#endif


#ifdef _WIN32
bool GetWinMajorMinorVersion(DWORD& major, DWORD& minor)
{
  bool bRetCode = false;
  LPBYTE pinfoRawData = 0;
  if (NERR_Success == NetWkstaGetInfo(NULL, 100, &pinfoRawData))
  {
    WKSTA_INFO_100* pworkstationInfo = (WKSTA_INFO_100*)pinfoRawData;
    major = pworkstationInfo->wki100_ver_major;
    minor = pworkstationInfo->wki100_ver_minor;
    ::NetApiBufferFree(pinfoRawData);
    bRetCode = true;
  }
  return bRetCode;
}

std::string os_info_impl::get_os_description() const
{
  std::string     winver;
  OSVERSIONINFOEX osver;
  SYSTEM_INFO     sysInfo;
  typedef void(__stdcall *GETSYSTEMINFO) (LPSYSTEM_INFO);

  __pragma(warning(push))
    __pragma(warning(disable:4996))
    memset(&osver, 0, sizeof(osver));
  osver.dwOSVersionInfoSize = sizeof(osver);
  GetVersionEx((LPOSVERSIONINFO)&osver);
  __pragma(warning(pop))
    DWORD major = 0;
  DWORD minor = 0;
  if (GetWinMajorMinorVersion(major, minor))
  {
    osver.dwMajorVersion = major;
    osver.dwMinorVersion = minor;
  }
  else if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 2)
  {
    OSVERSIONINFOEXW osvi;
    ULONGLONG cm = 0;
    cm = VerSetConditionMask(cm, VER_MINORVERSION, VER_EQUAL);
    ZeroMemory(&osvi, sizeof(osvi));
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    osvi.dwMinorVersion = 3;
    if (VerifyVersionInfoW(&osvi, VER_MINORVERSION, cm))
    {
      osver.dwMinorVersion = 3;
    }
  }

  GETSYSTEMINFO getSysInfo = (GETSYSTEMINFO)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "GetNativeSystemInfo");
  if (getSysInfo == NULL) {
    getSysInfo = ::GetSystemInfo;

  }
  getSysInfo(&sysInfo);

  if (osver.dwMajorVersion == 10 && osver.dwMinorVersion >= 0 && osver.wProductType != VER_NT_WORKSTATION)  winver = "Windows 10 Server";
  if (osver.dwMajorVersion == 10 && osver.dwMinorVersion >= 0 && osver.wProductType == VER_NT_WORKSTATION)  winver = "Windows 10";
  if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 3 && osver.wProductType != VER_NT_WORKSTATION)  winver = "Windows Server 2012 R2";
  if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 3 && osver.wProductType == VER_NT_WORKSTATION)  winver = "Windows 8.1";
  if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 2 && osver.wProductType != VER_NT_WORKSTATION)  winver = "Windows Server 2012";
  if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 2 && osver.wProductType == VER_NT_WORKSTATION)  winver = "Windows 8";
  if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 1 && osver.wProductType != VER_NT_WORKSTATION)  winver = "Windows Server 2008 R2";
  if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 1 && osver.wProductType == VER_NT_WORKSTATION)  winver = "Windows 7";
  if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 0 && osver.wProductType != VER_NT_WORKSTATION)  winver = "Windows Server 2008";
  if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 0 && osver.wProductType == VER_NT_WORKSTATION)  winver = "Windows Vista";
  if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 2 && osver.wProductType == VER_NT_WORKSTATION
    &&  sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)  winver = "Windows XP x64";
  if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 2)   winver = "Windows Server 2003";
  if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 1)   winver = "Windows XP";
  if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 0)   winver = "Windows 2000";
  if (osver.dwMajorVersion < 5)   winver = "unknown";

  if (osver.wServicePackMajor != 0)
  {
    std::string sp;
    char buf[128] = { 0 };
    sp = " Service Pack ";
    sprintf_s(buf, sizeof(buf), "%hd", osver.wServicePackMajor);
    sp.append(buf);
    winver = sp;
  }

  return winver;
}
#elif defined __linux__
std::string exec(const char* cmd) {
  char buffer[128];
  std::string result = "";
  std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
  if (!pipe) {
    std::string error_msg((const char *)strerror(errno));
    throw std::runtime_error(error_msg.c_str());

  }

  while (!feof(pipe.get())) {
    if (fgets(buffer, 128, pipe.get()) != NULL)
      result = buffer;

  }
  if (!result.empty() && result[result.length() - 1] == '\n') {
    result.erase(result.length() - 1);

  }
  return result;

}

std::string os_info_impl::get_os_description() const {
  std::string result;
  try {
    std::string name = exec("cat /etc/system-release");
    std::string os = exec("uname -s");
    std::string kernel = exec("uname -r");
    result = os + std::string(" ") + name + std::string(" (") + kernel + std::string(")");
  }
  catch (std::exception  &ex) {

  }
  return result;

}
#endif

BOOST_CLASS_EXPORT_IMPLEMENT(os_info_impl)
