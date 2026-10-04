#include "LoginForm.h"
#include "ThemeManager.h"
#include "Dashboard.h"
#include "AcademicSecurity.h"
#include "AcademicEditApprovals.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
int main()
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	SchoolCore::ThemeManager::Load();

	SchoolCore::LoginForm^ login =
		gcnew SchoolCore::LoginForm();

	if (login->ShowDialog() !=
		DialogResult::OK)
	{
		return 0;
	}

    if (SchoolCore::AuthSession::HasPermission(
            L"academic_records.override_inactive_year"))
    {
        int pendingApprovals =
            SchoolCore::AcademicSecurity::GetPendingApprovalCount();

        if (pendingApprovals > 0)
        {
            String^ requestText =
                pendingApprovals == 1
                ? L"There is 1 pending academic edit request requiring your independent approval."
                : L"There are " +
                  pendingApprovals.ToString() +
                  L" pending academic edit requests requiring your independent approval.";

            System::Windows::Forms::DialogResult reviewResult =
                MessageBox::Show(
                    requestText +
                    L"\r\n\r\nWould you like to review the requests now?",
                    L"SchoolCore | Pending Academic Approvals",
                    MessageBoxButtons::YesNo,
                    MessageBoxIcon::Information
                );

            if (reviewResult ==
                System::Windows::Forms::DialogResult::Yes)
            {
                SchoolCore::AcademicEditApprovals^ approvalForm =
                    gcnew SchoolCore::AcademicEditApprovals();

                approvalForm->ShowDialog();

                delete approvalForm;
            }
        }
    }

	Application::Run(
		gcnew SchoolCore::Dashboard()
	);

	return 0;
}
