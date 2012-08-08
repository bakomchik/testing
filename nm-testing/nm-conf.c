
/* Please remember, this is a testing repository. */

/* TODO: special care to see if WIRELESS is defined. */

#include "nm-conf.h"

#if NM_TEST_CONFIG
#include "global.h"
#endif


/* Functions for printing informations in Network Manager format. */

void nm_write_connection(FILE *config_file, nm_connection connection)
{
    fprintf(config_file, "\n%s\n", NM_SETTINGS_CONNECTION);
    fprintf(config_file, "id=%s\n", connection.id);
    fprintf(config_file, "uuid=%s\n", connection.uuid);
    fprintf(config_file, "type=%s\n", (connection.type == WIRELESS) ?
            NM_DEFAULT_WIRELESS : NM_DEFAULT_WIRED);
}

void nm_write_wireless_specific_options(FILE *config_file,
        nm_wireless wireless)
{
    fprintf(config_file, "\n%s\n", NM_SETTINGS_WIRELESS);
    fprintf(config_file, "ssid=%s\n", wireless.ssid);
    fprintf(config_file, "mode=%s\n", (wireless.mode == AD_HOC) ?
            "adhoc" : "infrastructure");
    fprintf(config_file, "mac=%s\n", wireless.mac_addr);

    if (wireless.is_secured == TRUE) {
        fprintf(config_file, "security=%s\n", NM_DEFAULT_WIRELESS_SECURITY);
    }
}

void nm_write_wireless_security(FILE *config_file, nm_wireless_security
        wireless_security) //TODO
{

}

void nm_write_ipv4(FILE *config_file, nm_ipv4 ipv4) // TODO
{

}

void nm_write_ipv6(FILE *config_file, nm_ipv6 ipv6)
{
    fprintf(config_file, "\n%s\n", NM_SETTINGS_IPV6);
    fprintf(config_file, "method=%s\n", "ignore");
}

/* Write Network Manager config file. */
void nm_write_config_file(struct nm_config_info nmconf)
{
    FILE *config_file = fopen(nmconf.connection.id, "w");

    if (config_file == NULL) {
        fprintf(stderr, "Unable to open file for writting configurations, "
                        "connection id might not be set or set to unproper "
                        "value. Current value: %s\n", nmconf.connection.id);
        return;
    }

    nm_write_connection(config_file, nmconf.connection);

    if (nmconf.connection.type == WIRELESS) {
        nm_write_wireless_specific_options(config_file, nmconf.wireless);
        if (nmconf.wireless.is_secured) {
            nm_write_wireless_security(config_file, nmconf.wireless_security);
        }
    }

    nm_write_ipv4(config_file, nmconf.ipv4);
    nm_write_ipv6(config_file, nmconf.ipv6);

    fclose(config_file);
}

/* Functions for extracting information from netcfg variables. */

/* Get info for the connection setting. */
void nm_get_wireless_connection(nm_connection *connection)
{
    snprintf(connection->id, NM_MAX_LEN_ID, "Auto %s", essid);
    uuid4_generate(connection->uuid);
    connection->type = WIRELESS;
}


void nm_get_wireless_specific_options(nm_wireless *wireless)
{
    strncpy(wireless->ssid, essid, NM_MAX_LEN_SSID);

    /* Get MAC address from default file. */
    char file_name[NM_MAX_LEN_PATH];
    FILE *file;

    snprintf(file_name, NM_MAX_LEN_PATH, NM_DEFAULT_PATH_FOR_MAC, interface);
    file = fopen(file_name, "r");

    if (file == NULL) {
        wireless->mac_addr[0] = '\0';   /* Empty string means no MAC. */
    }
    else {
        fscanf(file, "%s\n", wireless->mac_addr);

        /* Should be upper case. */
        int i;
        for (i = 0; wireless->mac_addr[i]; i++) {
            wireless->mac_addr[i] = toupper(wireless->mac_addr[i]);
        }
    }

    /* Decide mode. */
    if (mode == ADHOC) {
        wireless->mode = AD_HOC;
    }
    else {
        wireless->mode = INFRASTRUCTURE;
    }

    /* In netcfg, you have to chose WEP and leave the key empty for an
     * unsecure connection. */
    if (wifi_security == WEP &&  wepkey == NULL) {
        wireless->is_secured = FALSE;
    }
    else {
        wireless->is_secured = TRUE;
    }
}

/* Security type for wireless networks. */
void nm_get_wireless_security(nm_wireless_security *wireless_security) //TODO
{

}

/* Save IPv4 settings. */
void nm_get_ipv4(nm_ipv4 *ipv4) // TODO
{

}

/* For the moment, just set it to ignore. */
void nm_get_ipv6(nm_ipv6 *ipv6)
{
    ipv6->method = IGNORE;
}

/* Extract all configs for a wireless interface, from both global netcfg
 * values and other resources. */
void nm_get_wireless_config(struct nm_config_info *nmconf)
{
    nm_get_wireless_connection(&(nmconf->connection));
    nm_get_wireless_specific_options(&(nmconf->wireless));

    if (nmconf->wireless.is_secured == TRUE) {
        nm_get_wireless_security(&(nmconf->wireless_security));
    }

    nm_get_ipv4(&(nmconf->ipv4));
    nm_get_ipv6(&(nmconf->ipv6));
}

/* Relies on global variables (y u no say? :) ) */
void nm_get_configuration(struct nm_config_info *nmconf)
{
    /* Decide if wireless configuration is needed. */
    if (is_wireless_iface(interface)) {
        nm_get_wireless_config(nmconf);
    }
}