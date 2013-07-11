/// \file main.cpp

#include <QApplication>
#include <QTimer>
#include <memory>

#include "main_window.h"

#include "common/memory_deallocation.h"
#include "common/common_utils.h"

#include "console/console.h"
#include "console/console_task.h"
#include "console/gengetopt/cl_arguments_class.h"



#ifdef _WINDOWS
#include <consoleapi.h>
#endif // _WINDOWS


int main (int argc, char **argv)
{
  QApplication app (argc, argv);
  std::unique_ptr<main_window> mainWin;
  std::unique_ptr<cl_arguments> args (new cl_arguments);
  bool console_inited;

  if (app.arguments ().size () == 1) // If QT eaten all arguments, then it means that program needs to run GUI
    {
      mainWin.reset (new main_window);
      mainWin->show ();
      
    }
  else
    {
#ifdef _WINDOWS
      DO_ON_EXIT ([] () {
         fclose(stdout);
         FreeConsole();
      }
      );
      if (!AttachConsole (-1))
        console_inited = AllocConsole ();
      if (!console_inited)
        goto cleanup;
      freopen("CONOUT$", "wb", stdout);     
#endif // _WINDOWS
      if (args->init (argc, argv))
        return 0;

      console_task *task = new console_task (&app, args.get ());
      QTimer::singleShot (0, task, SLOT (run ()));
    }

  return app.exec ();
cleanup:
  if (console_inited)   // Actually if gengetopt exited this cleanup won't be mad
    {                   // For now let's ignore it
      fclose(stdout);
      FreeConsole();
    }
}
