#include <stdio.h>
#include <fcntl.h>
#include <syslog.h>

#include "gen/interface.h"

void LOG(const char *fmt, ...) {
  char buf[0x1000];
  va_list va;
  va_start(va, fmt);
  vsnprintf(buf, sizeof(buf), fmt, va);
  va_end(va);
  syslog(LOG_INFO, buf);
}

static gboolean on_get_flag(MyServer *server,
                            GDBusMethodInvocation *invocation,
                            gchar *message,
                            gpointer user_data) {
  if (strcmp(message, "hello") != 0) {
    g_dbus_method_invocation_return_value(
      invocation,
      g_variant_new("(s)", "knock knock, who's there? (See interface.xml)")
    );
    goto out;
  }

  char flag[0x100];
  FILE *fp = fopen("/flag.txt", "r");
  fgets(flag, 0x100, fp);

  LOG("MyServer :: GetFlag()");

  g_dbus_method_invocation_return_value(
    invocation,
    g_variant_new("(s)", flag)
  );

out:
  return TRUE; /* to indicate that the method was handled */
}

static void on_bus_acquired(GDBusConnection *connection, const gchar *name,
                            gpointer user_data) {
  LOG("MyServer :: Acquired a message bus connection from %s!", name);

  MyServer *object = my_server_skeleton_new();

  g_signal_connect(object, "handle-get-flag",
                   G_CALLBACK(&on_get_flag), NULL);

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

void start_server() {
  GMainLoop *loop;
  loop = g_main_loop_new(NULL, FALSE);

  guint id = g_bus_own_name(
      G_BUS_TYPE_SYSTEM, "org.babydbus.MyServer",
      G_BUS_NAME_OWNER_FLAGS_ALLOW_REPLACEMENT | G_BUS_NAME_OWNER_FLAGS_REPLACE,
      on_bus_acquired, on_name_acquired, on_name_lost, loop, NULL);

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