#include "LoginForm.h"
#include "Dashboard.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
int main()
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	SchoolCore::LoginForm^ login =
		gcnew SchoolCore::LoginForm();

	if (login->ShowDialog() !=
		DialogResult::OK)
	{
		return 0;
	}

	Application::Run(
		gcnew SchoolCore::Dashboard()
	);

	return 0;
}
