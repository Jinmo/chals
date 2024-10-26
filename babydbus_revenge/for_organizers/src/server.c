#include <stdio.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "gen/interface.h"

void LOG(const char *fmt, ...) {
  char buf[0x1000] = {0};
  va_list va;
  va_start(va, fmt);
  vsnprintf(buf, sizeof(buf), fmt, va);
  va_end(va);
  syslog(LOG_INFO, buf);
}

gboolean ishexdigit(char c) {
  return ('0' <= c && c <= '9') || (
    'A' <= c && c <= 'F'
    ) || (
    'a' <= c && c <= 'f'
    );
}

void urldecode(char *str) {
  // a%FFb -> a\xffb
  int i = 0, j = 0;
  while(str[i] != '\0') {
    if(str[i] == '%' && ishexdigit(str[i + 1]) && ishexdigit(str[i + 2])) {
      char tmp[3] = {str[i + 1], str[i + 2], '\0'};
      sscanf(tmp, "%hhx", str + j);
      j++;
      i += 3;
    }
    else {
      str[j] = str[i];
      i++;
      j++;
    }
  }
  str[j] = '\0';
}

static gboolean on_report(MyServer *server,
                          GDBusMethodInvocation *invocation,
                          gchar *reporter,
                          gpointer user_data) {
  urldecode(reporter);

  LOG("Request from %s", reporter);
  g_dbus_method_invocation_return_value(invocation,
    g_variant_new("(s)", "Hello from MyServer!")
  );

out:
  return TRUE; /* to indicate that the method was handled */
}

static void on_bus_acquired(GDBusConnection *connection, const gchar *name,
                            gpointer user_data) {
  g_print("Acquired a message bus connection\n");

  MyServer *object = my_server_skeleton_new();

  g_signal_connect(object, "handle-report",
                   G_CALLBACK(&on_report), NULL);

  g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(object),
                                   connection, "/babydbus/MyServer", NULL);
}

static void on_name_acquired(GDBusConnection *connection, const gchar *name,
                             gpointer user_data) {
  LOG("Acquired the name %s", name);
}

static void on_name_lost(GDBusConnection *connection, const gchar *name,
                         gpointer user_data) {
  LOG("Lost the name %s", name);
}

gboolean sync_to_disk(gpointer data) {
  static char sync_command[0x100] = "sync";

  puts("Syncing to disk...");
  system(sync_command);

  return TRUE;
}

void start_server() {
  GMainLoop *loop;
  loop = g_main_loop_new(NULL, FALSE);

  guint id = g_bus_own_name(
      G_BUS_TYPE_SYSTEM, "org.babydbus.MyServer",
      G_BUS_NAME_OWNER_FLAGS_ALLOW_REPLACEMENT | G_BUS_NAME_OWNER_FLAGS_REPLACE,
      on_bus_acquired, on_name_acquired, on_name_lost, loop, NULL);

  g_timeout_add_seconds(5, sync_to_disk, NULL);

  g_main_loop_run(loop);
  g_bus_unown_name(id);
  g_main_loop_unref(loop);
}

gint main(gint argc, gchar *argv[]) {
  // Run on background
  if(fork()) return 0;

  // Restart on crash
  int pid;
  while(1) {
    if(pid = fork()) {
      if(pid < 0) {
        perror("fork");
        return 1;
      }
      if(waitpid(pid, NULL, 0) == -1) {
        perror("waitpid");
        return 1;
      }
    } else break;
  }

  start_server();

  return 0;
}