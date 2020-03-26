#include <stdio.h>
#include <stdlib.h>
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>


int main(int argc, char *argv[])
{
    virConnectPtr conn;

    

    conn = virConnectOpen("qemu:///system");
    if (conn == NULL) {
        fprintf(stderr, "Failed to open connection to qemu:///system\n");
        return 1;
    }
    const char *xml ="<volume>\n\
 <name>server_state.img</name>\n\
 <capacity unit=\"G\">10</capacity>\n\
</volume>";
    virDomainPtr dom;

    dom = virDomainDefineXML(conn, xml);
    if (!dom) {
        fprintf(stderr, "Unable to define persistent guest configuration");
        return -1;
    }

    if (virDomainCreate(dom) < 0) {
        fprintf(stderr, "Unable to boot guest configuration");
    }

    virConnectClose(conn);
    return 0;
}