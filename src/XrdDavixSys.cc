
#include "XrdDavix.hh"


XrdDavixSys g_storage;
Davix::Context XrdDavixSys::m_context;


extern "C"
{
XrdOss *XrdOssGetStorageSystem(XrdOss       *native_oss,
                               XrdSysLogger *Logger,
                               const char   *config_fname,
                               const char   *parms)
{
    if (g_storage.Init(Logger, config_fname))
    {
        return NULL;
    }
    return &g_storage;
}
}


XrdDavixSys::XrdDavixSys()
 : XrdOss(),
   m_posix(&m_context)
{
}


XrdDavixSys::~XrdDavixSys()
{
}


int
XrdDavixSys::Mkdir(const char *dirname, mode_t mode, int opts, XrdOucEnv *ep)
{
    Davix::DavixError *err = NULL;
    std::string url;
    Davix::RequestParams *params;
    if (int result = CreateUrl(dirname, params, url)) {return result;}
    return ConvertErr(m_posix.mkdir(NULL, dirname, mode, &err), err);
}


int
XrdDavixSys::Remdir(const char *dirname, int opts, XrdOucEnv *ep)
{
    Davix::DavixError *err = NULL;
    std::string url;
    Davix::RequestParams *params;
    if (int result = CreateUrl(dirname, params, url)) {return result;}
    return ConvertErr(m_posix.rmdir(NULL, dirname, &err), err);
}


int
XrdDavixSys::Rename(const char *srcpath, const char *destpath, XrdOucEnv *ep1, XrdOucEnv *ep2)
{
    Davix::DavixError *err = NULL;
    std::string url1, url2;
    Davix::RequestParams *params1, *params2;
    if (int result = CreateUrl(srcpath, params1, url1)) {return result;}
    if (int result = CreateUrl(destpath, params2, url2)) {return result;}
    return ConvertErr(m_posix.rename(params1, url1, url2, &err), err);
}


int
XrdDavixSys::Stat(const char *path, struct stat *st, int opts, XrdOucEnv *ep)
{
    Davix::DavixError *err = NULL;
    Davix::RequestParams *params;
    std::string url;
    if (int result = CreateUrl(path, params, url)) {return result;}
    int retval = ConvertErr(m_posix.stat(params, url, st, &err), err);
    if (err)
    {
        m_edest->Emsg("XrdDavixSys", "Failed to stat:", err->getErrMsg().c_str());
    }
    return retval;
}


int
XrdDavixSys::Unlink(const char *path, int opts, XrdOucEnv *ep)
{
    Davix::DavixError *err = NULL;
    Davix::RequestParams *params;
    std::string url;
    if (int result = CreateUrl(path, params, url)) {return result;}
    return ConvertErr(m_posix.unlink(params, path, &err), err);
}


int
XrdDavixSys::Init(XrdSysLogger *lp, const char *configfn)
{
    m_edest = new XrdSysError(0, "davix_");
    m_edest->logger(lp);
    m_edest->Say("DAVIX-based HTTP proxy initialized.");

    return Config(configfn);
}


int
XrdDavixSys::CreateUrl(const char *path, Davix::RequestParams *&params, std::string &url)
{
    url = m_prefix + "/" + path;
    params = NULL;

    return 0;
}


int
XrdDavixSys::ConvertErr(int errcode, Davix::DavixError *err)
{
    if (errcode == 0) {return 0;}

    if (!err) {return -EIO;}

    switch (err->getStatus())
    {
    case Davix::StatusCode::OK:
        return -EIO;
    case Davix::StatusCode::AuthenticationError:
    case Davix::StatusCode::LoginPasswordError:
    case Davix::StatusCode::CredentialNotFound:
    case Davix::StatusCode::PermissionRefused:
        return -EACCES;
    case Davix::StatusCode::IsADirectory:
        return -EISDIR;
    case Davix::StatusCode::FileExist:
        return -EEXIST;
    case Davix::StatusCode::InvalidArgument:
        return -EINVAL;
    case Davix::StatusCode::TimeoutRedirectionError:
        return -ETIMEDOUT;
    case Davix::StatusCode::OperationNonSupported:
        return -ENOTSUP;
    case Davix::StatusCode::FileNotFound:
        return -ENOENT;
    default:
        return -EIO;
    }
}


