#ifndef __XRD_DAVIX_HH_
#define __XRD_DAVIX_HH_

#include "XrdOss/XrdOss.hh"
#include "XrdOuc/XrdOucErrInfo.hh"

#include <string>

#include "davix.hpp"

class XrdSfsAio;

class XrdDavixSys;

class XrdDavixDirectory : public XrdOssDF
{
friend class XrdDavixSys;

public:

  XrdOucErrInfo  error;

  virtual int Opendir(const char *dirName, XrdOucEnv &);
  virtual int Readdir(char *buff, int blen);
  virtual int Close(long long *retsz=0);

private:
  XrdDavixDirectory(const char *tid, XrdDavixSys &sys);
  virtual ~XrdDavixDirectory();

  XrdDavixSys &m_sys;
  Davix::DavPosix m_posix;
  DAVIX_DIR *m_dirp;
};

class XrdDavixFile : public XrdOssDF
{
friend class XrdDavixSys;

public:

  XrdOucErrInfo  error;

  virtual int Open(const char *fileName,
                   int         openMode,
                   mode_t      createMode,
                   XrdOucEnv  &client);

  virtual int Close(long long *retsz=0);

  virtual int Fstat(struct stat *);

  virtual int Fsync() {return -ENOTSUP;}

  virtual int Fsync(XrdSfsAio *a) {return -ENOTSUP;}

  virtual off_t getMmap(void **Addr) {*Addr = 0; return 0;}

  virtual ssize_t Read(off_t fileOffset, size_t preread_sz) {return XrdOssOK;}

  virtual ssize_t Read(void  *buffer,
                       off_t  fileOffset,
                       size_t buffer_size);

  virtual int Read(XrdSfsAio *aioparm);

  virtual ssize_t ReadRaw(void  *buffer,
                          off_t  fileOffset,
                          size_t buffer_size) {return Read(buffer, fileOffset, buffer_size);}

  virtual ssize_t Write(const void *buffer,
                        off_t       fileOffset,
                        size_t      buffer_size) {return -ENOTSUP;}

  virtual int Write(XrdSfsAio *aioparm) {return -ENOTSUP;}

  virtual int Ftruncate(unsigned long long) {return -ENOTSUP;}

private:
  XrdDavixFile(const char *user, XrdDavixSys &sys);
  virtual ~XrdDavixFile();

  XrdDavixSys &m_sys;
  Davix::DavPosix m_posix;
  DAVIX_FD *m_fd;
  std::string m_url;
};


class XrdDavixSys : public XrdOss
{
public:
  XrdOucErrInfo  error;

  virtual int Init(XrdSysLogger *, const char *);

  virtual XrdOssDF *newDir(const char *user=0) {return new XrdDavixDirectory(user, *this);}
  virtual XrdOssDF *newFile(const char *user=0) {return new XrdDavixFile(user, *this);}

  virtual int Chmod(const char *path, mode_t mode, XrdOucEnv *ep=0) {return -ENOTSUP;}

  virtual int Create(const char *path, const char *, mode_t, XrdOucEnv &, int opts=0) {return -ENOTSUP;}

  virtual int Mkdir(const char* dirname, mode_t mode, int mkpath=0, XrdOucEnv *env=0);

  int Remdir(const char *path, int opts=0, XrdOucEnv *env=0);

  int Rename(const char *srcpath, const char *destpath, XrdOucEnv *ep1, XrdOucEnv *ep2);

  int Stat(const char *, struct stat *, int resonly=0, XrdOucEnv* env=0);

  virtual int Truncate(const char*, long long unsigned int, XrdOucEnv *ep=0) {return -ENOTSUP;}

  int Unlink(const char *path, int opts=0, XrdOucEnv *ep=0);

  Davix::Context &getContext() {return m_context;}

  int CreateUrl(const char *path, Davix::RequestParams *&params, std::string &url);

  static int ConvertErr(int errcode, Davix::DavixError *err);

  XrdDavixSys();
  virtual ~XrdDavixSys();

  XrdSysError *m_edest;

private:

  int Config(const char *configfn);

  std::string m_prefix;

  Davix::DavPosix m_posix;
  static Davix::Context m_context;
};

#endif
