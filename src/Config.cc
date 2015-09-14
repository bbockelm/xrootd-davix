
#include "XrdDavix.hh"

#include "XrdOuc/XrdOucEnv.hh"
#include "XrdOuc/XrdOucStream.hh"

int
XrdDavixSys::Config(const char *configfn)
{
    m_edest->Emsg("Config", "Configuring XrdDavix");

    XrdOucEnv myEnv;
    XrdOucStream Config(m_edest, getenv("XRDINSTANCE"), &myEnv, "=====> ");

    if (!configfn || !*configfn)
    {
        m_edest->Emsg("Config", "Configuration file not specified.");
        return 1;
    }

    int fd;
    if ( (fd = open(configfn, O_RDONLY, 0)) < 0)
    {
        m_edest->Emsg("Config", errno, "open config file", configfn);
        return 1;
    }
    Config.Attach(fd);

    const char *var, *val;
    while ((var = Config.GetMyFirstWord()))
    {
        if (!strcmp("davix.prefix", var))
        {
            if (!(val = Config.GetWord()) || !val[0])
            {
                m_edest->Emsg("Config", "davix.prefix must have a prefix as an argument.");
                return 1;
            }
            m_prefix = val;
            m_edest->Emsg("Config", "DAVIX prefix is", val);
        }
    }
    if (!m_prefix.size())
    {
        m_edest->Emsg("Config", "davix.prefix not specified.");
        return 1;
    }

    return 0;
}


