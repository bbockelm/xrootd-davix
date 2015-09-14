
#include <stdio.h>
#include <sstream>

#include "XrdDavix.hh"

#include "XrdSfs/XrdSfsAio.hh"


XrdDavixFile::XrdDavixFile(const char *tid, XrdDavixSys &sys)
 : XrdOssDF(),
   m_sys(sys),
   m_posix(&sys.getContext()),
   m_fd(NULL)
{
}


XrdDavixFile::~XrdDavixFile() {}


int
XrdDavixFile::Open(const char *fileName,
                   int         openMode,
                   mode_t      createMode,
                   XrdOucEnv  &client)
{
    Davix::DavixError* err = NULL;
    Davix::RequestParams *params;
    if (int result = m_sys.CreateUrl(fileName, params, m_url)) {return result;}
    m_fd = m_posix.open(params, m_url, O_RDONLY, &err);
    if (!m_fd) {m_sys.m_edest->Emsg("DavixFile", "Error in opening url", m_url.c_str(), err->getErrMsg().c_str());}
    return m_fd ? 0 : XrdDavixSys::ConvertErr(-1, err);
}


int
XrdDavixFile::Close(long long *)
{
    Davix::DavixError* err = NULL;
    return XrdDavixSys::ConvertErr(m_posix.close(m_fd, &err), err);
}


int
XrdDavixFile::Fstat(struct stat * st)
{
    Davix::DavixError* err = NULL;
    Davix::RequestParams *params;
    std::string url;
    if (int result = m_sys.CreateUrl("", params, url)) {return result;}
    int result = m_posix.stat(NULL, m_url, st, &err);
    if (-1 == result) {m_sys.m_edest->Emsg("DavixFile", "Failure when stating URL", m_url.c_str(), err->getErrMsg().c_str());}
    return result;
}


int
XrdDavixFile::Read(XrdSfsAio *aiop)
{
   aiop->Result = this->Read((void *)aiop->sfsAio.aio_buf, aiop->sfsAio.aio_offset,
                             aiop->sfsAio.aio_nbytes);
   aiop->doneRead();
   return 0;
}


ssize_t
XrdDavixFile::Read(void  *buffer,
                   off_t  fileOffset,
                   size_t buffer_size)
{
    Davix::DavixError* err = NULL;
    int retval = m_posix.pread(m_fd, buffer, buffer_size, fileOffset, &err);
    if (-1 ==retval) {m_sys.m_edest->Emsg("DavixFile", "Failure when reading URL", m_url.c_str(), err->getErrMsg().c_str());}
    return retval;
}


XrdDavixDirectory::XrdDavixDirectory(const char *tid, XrdDavixSys &sys)
 : XrdOssDF(),
   m_sys(sys),
   m_posix(&sys.getContext()),
   m_dirp(NULL)
{
}


XrdDavixDirectory::~XrdDavixDirectory()
{
}


int
XrdDavixDirectory::Opendir(const char *dirName, XrdOucEnv &)
{
    Davix::DavixError* err = NULL;
    Davix::RequestParams *params = NULL;
    std::string url;
    if (int result = m_sys.CreateUrl(dirName, params, url)) {return result;}

    m_dirp = m_posix.opendir(params, url, &err);
    if (!m_dirp) {m_sys.m_edest->Emsg("DavixDirectory", "Failure when listing URL", url.c_str(), err->getErrMsg().c_str());}
    return m_dirp ? 0 : XrdDavixSys::ConvertErr(-1, err);
}


int
XrdDavixDirectory::Readdir(char *buff, int blen)
{
    Davix::DavixError* err = NULL;
    struct dirent* entry;
    entry = m_posix.readdir(m_dirp, &err);
    if (!entry) {m_sys.m_edest->Emsg("DavixDirectory", "Failed when reading directory entries", err->getErrMsg().c_str());}
    if (!entry || (snprintf(buff, blen, "%s", entry->d_name) >= blen))
    {
        if (blen > 0) {buff[0] = '\0';}
    }
    return 0;
}


int
XrdDavixDirectory::Close(long long *)
{
    Davix::DavixError *err = NULL;
    return m_posix.closedir(m_dirp, &err);
}


