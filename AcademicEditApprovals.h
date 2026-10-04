#pragma once

#include "DbConnection.h"
#include "ThemeManager.h"
#include "AuthSession.h"

#include <mariadb/conncpp.hpp>
#include <msclr/marshal_cppstd.h>

#include <memory>
#include <string>

using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;

namespace SchoolCore
{
    public ref class AcademicEditApprovals : public Form
    {
    private:
        DataGridView^ approvalsGrid;
        Button^ btnRefresh;
        Button^ btnApprove;
        Button^ btnReject;
        Button^ btnClose;
        Label^ lblTitle;
        Label^ lblSubtitle;
        Label^ lblCount;

        int GetSelectedApprovalId()
        {
            if (this->approvalsGrid == nullptr ||
                this->approvalsGrid->SelectedRows->Count == 0)
            {
                return 0;
            }

            Object^ value =
                this->approvalsGrid
                    ->SelectedRows[0]
                    ->Cells["ApprovalId"]
                    ->Value;

            return value == nullptr
                ? 0
                : Convert::ToInt32(value);
        }

        void ShowDatabaseError(sql::SQLException& ex)
        {
            MessageBox::Show(
                gcnew String(ex.what()),
                L"Academic Edit Approvals",
                MessageBoxButtons::OK,
                MessageBoxIcon::Error
            );
        }

        void UpdateActionState()
        {
            bool selected =
                this->approvalsGrid->SelectedRows->Count > 0;

            this->btnApprove->Enabled = selected;
            this->btnReject->Enabled = selected;
        }

        void LoadApprovals()
        {
            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::Statement> stmt(
                    con->createStatement()
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery(
                        "SELECT "
                        "a.approval_id, "
                        "ay.year_name, "
                        "COALESCE(t.term_name, '') AS term_name, "
                        "a.operation, "
                        "a.table_name, "
                        "COALESCE(a.record_id, '') AS record_id, "
                        "a.reason, "
                        "COALESCE(u.full_name, u.username, '') AS requester, "
                        "a.status, "
                        "a.requested_at, "
                        "COALESCE(ap.full_name, ap.username, '') AS approver "
                        "FROM academic_edit_approvals a "
                        "INNER JOIN academic_years ay "
                        "ON ay.academic_year_id = a.academic_year_id "
                        "LEFT JOIN terms t "
                        "ON t.term_id = a.term_id "
                        "INNER JOIN users u "
                        "ON u.user_id = a.requested_by "
                        "LEFT JOIN users ap "
                        "ON ap.user_id = a.approved_by "
                        "ORDER BY "
                        "CASE WHEN a.status = 'Pending' THEN 0 ELSE 1 END, "
                        "a.approval_id DESC"
                    )
                );

                this->approvalsGrid->Rows->Clear();

                while (result->next())
                {
                    String^ requester =
                        gcnew String(
                            result->getString("requester").c_str()
                        );

                    String^ approver =
                        gcnew String(
                            result->getString("approver").c_str()
                        );

                    String^ requestedAt =
                        result->isNull("requested_at")
                        ? L""
                        : gcnew String(
                            result->getString("requested_at").c_str()
                        );

                    this->approvalsGrid->Rows->Add(
                        result->getInt("approval_id"),
                        gcnew String(result->getString("year_name").c_str()),
                        gcnew String(result->getString("term_name").c_str()),
                        gcnew String(result->getString("operation").c_str()),
                        gcnew String(result->getString("table_name").c_str()),
                        gcnew String(result->getString("record_id").c_str()),
                        gcnew String(result->getString("reason").c_str()),
                        requester,
                        gcnew String(result->getString("status").c_str()),
                        requestedAt,
                        approver
                    );
                }

                this->lblCount->Text =
                    L"Requests: " +
                    this->approvalsGrid->Rows->Count.ToString();

                UpdateActionState();
            }
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
            }
        }

        void SetApprovalStatus(
            int approvalId,
            String^ newStatus)
        {
            if (approvalId <= 0)
                return;

            if (!AuthSession::HasPermission(
                    L"academic_records.override_inactive_year"))
            {
                MessageBox::Show(
                    L"You do not have permission to approve or reject academic-period override requests.",
                    L"Access Denied",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );
                return;
            }

            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> check(
                    con->prepareStatement(
                        "SELECT requested_by, status "
                        "FROM academic_edit_approvals "
                        "WHERE approval_id = ? "
                        "LIMIT 1"
                    )
                );

                check->setInt(1, approvalId);

                std::unique_ptr<sql::ResultSet> result(
                    check->executeQuery()
                );

                if (!result->next())
                {
                    MessageBox::Show(
                        L"The selected approval request could not be found.",
                        L"Academic Edit Approvals",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning
                    );
                    return;
                }

                int requestedBy =
                    result->getInt("requested_by");

                String^ currentStatus =
                    gcnew String(
                        result->getString("status").c_str()
                    );

                if (!currentStatus->Equals(
                        L"Pending",
                        StringComparison::OrdinalIgnoreCase))
                {
                    MessageBox::Show(
                        L"Only Pending requests can be approved or rejected.",
                        L"Academic Edit Approvals",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Information
                    );
                    return;
                }

                if (requestedBy == AuthSession::UserId)
                {
                    MessageBox::Show(
                        L"You cannot approve or reject your own emergency edit request. A different authorized approver is required.",
                        L"Independent Approval Required",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning
                    );
                    return;
                }

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "UPDATE academic_edit_approvals "
                        "SET status = ?, "
                        "approved_by = ?, "
                        "approved_at = CURRENT_TIMESTAMP "
                        "WHERE approval_id = ? "
                        "AND status = 'Pending' "
                        "AND requested_by <> ?"
                    )
                );

                stmt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(
                        newStatus
                    )
                );

                stmt->setInt(
                    2,
                    AuthSession::UserId
                );

                stmt->setInt(
                    3,
                    approvalId
                );

                stmt->setInt(
                    4,
                    AuthSession::UserId
                );

                int affected =
                    stmt->executeUpdate();

                if (affected != 1)
                {
                    MessageBox::Show(
                        L"The approval request was not changed. It may already have been processed.",
                        L"Academic Edit Approvals",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning
                    );
                    return;
                }

                LoadApprovals();

                MessageBox::Show(
                    newStatus->Equals(
                        L"Approved",
                        StringComparison::OrdinalIgnoreCase)
                    ? L"The emergency edit request was approved."
                    : L"The emergency edit request was rejected.",
                    L"Academic Edit Approvals",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );
            }
            catch (sql::SQLException& ex)
            {
                ShowDatabaseError(ex);
            }
        }

        Void RefreshClicked(
            Object^,
            EventArgs^)
        {
            LoadApprovals();
        }

        Void ApproveClicked(
            Object^,
            EventArgs^)
        {
            int id = GetSelectedApprovalId();

            if (id <= 0)
                return;

            if (MessageBox::Show(
                    L"Approve this emergency academic-record edit request?",
                    L"Confirm Approval",
                    MessageBoxButtons::YesNo,
                    MessageBoxIcon::Question) !=
                DialogResult::Yes)
            {
                return;
            }

            SetApprovalStatus(
                id,
                L"Approved"
            );
        }

        Void RejectClicked(
            Object^,
            EventArgs^)
        {
            int id = GetSelectedApprovalId();

            if (id <= 0)
                return;

            if (MessageBox::Show(
                    L"Reject this emergency academic-record edit request?",
                    L"Confirm Rejection",
                    MessageBoxButtons::YesNo,
                    MessageBoxIcon::Question) !=
                DialogResult::Yes)
            {
                return;
            }

            SetApprovalStatus(
                id,
                L"Rejected"
            );
        }

        Void CloseClicked(
            Object^,
            EventArgs^)
        {
            this->Close();
        }

        void ConfigureGrid()
        {
            this->approvalsGrid =
                gcnew DataGridView();

            this->approvalsGrid->Dock =
                DockStyle::Fill;

            this->approvalsGrid->AllowUserToAddRows = false;
            this->approvalsGrid->AllowUserToDeleteRows = false;
            this->approvalsGrid->ReadOnly = true;
            this->approvalsGrid->MultiSelect = false;
            this->approvalsGrid->SelectionMode =
                DataGridViewSelectionMode::FullRowSelect;
            this->approvalsGrid->RowHeadersVisible = false;
            this->approvalsGrid->AutoGenerateColumns = false;
            this->approvalsGrid->BorderStyle = BorderStyle::None;
            this->approvalsGrid->BackgroundColor = Color::White;
            this->approvalsGrid->ColumnHeadersHeight = 40;
            this->approvalsGrid->RowTemplate->Height = 42;

            AddColumn(L"ApprovalId", L"ID", 65);
            AddColumn(L"AcademicYear", L"Academic Year", 105);
            AddColumn(L"Term", L"Term", 100);
            AddColumn(L"Operation", L"Operation", 125);
            AddColumn(L"Table", L"Table", 120);
            AddColumn(L"RecordId", L"Record", 80);
            AddColumn(L"Reason", L"Reason", 280);
            AddColumn(L"Requester", L"Requester", 170);
            AddColumn(L"Status", L"Status", 95);
            AddColumn(L"RequestedAt", L"Requested At", 145);
            AddColumn(L"Approver", L"Approver", 170);

            this->approvalsGrid->SelectionChanged +=
                gcnew EventHandler(
                    this,
                    &AcademicEditApprovals::GridSelectionChanged
                );
        }

        DataGridViewTextBoxColumn^ AddColumn(
            String^ name,
            String^ header,
            int width)
        {
            DataGridViewTextBoxColumn^ column =
                gcnew DataGridViewTextBoxColumn();

            column->Name = name;
            column->HeaderText = header;
            column->Width = width;
            column->SortMode =
                DataGridViewColumnSortMode::NotSortable;

            if (name->Equals(L"Reason"))
            {
                column->AutoSizeMode =
                    DataGridViewAutoSizeColumnMode::Fill;
            }

            this->approvalsGrid->Columns->Add(column);

            return column;
        }

        Void GridSelectionChanged(
            Object^,
            EventArgs^)
        {
            UpdateActionState();
        }

        void InitializeComponent()
        {
            this->SuspendLayout();

            this->Text =
                L"SchoolCore | Academic Edit Approvals";

            this->StartPosition =
                FormStartPosition::CenterParent;

            this->ClientSize =
                Drawing::Size(1320, 720);

            this->MinimumSize =
                Drawing::Size(1100, 620);

            this->BackColor =
                Color::FromArgb(248, 250, 252);

            this->Font =
                gcnew Drawing::Font(
                    L"Segoe UI",
                    9.5F
                );

            TableLayoutPanel^ root =
                gcnew TableLayoutPanel();

            root->Dock = DockStyle::Fill;
            root->ColumnCount = 1;
            root->RowCount = 4;
            root->Padding =
                System::Windows::Forms::Padding(20);

            root->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    68.0F));

            root->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    52.0F));

            root->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Percent,
                    100.0F));

            root->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    54.0F));

            Panel^ header =
                gcnew Panel();

            header->Dock = DockStyle::Fill;
            header->BackColor = Color::White;
            header->Padding =
                System::Windows::Forms::Padding(18, 8, 18, 8);

            this->lblTitle =
                gcnew Label();

            this->lblTitle->Text =
                L"Academic Edit Approvals";

            this->lblTitle->Dock = DockStyle::Top;
            this->lblTitle->Height = 30;
            this->lblTitle->Font =
                gcnew Drawing::Font(
                    L"Segoe UI Semibold",
                    17.0F,
                    FontStyle::Bold);

            this->lblTitle->ForeColor =
                Color::FromArgb(15, 23, 42);

            this->lblSubtitle =
                gcnew Label();

            this->lblSubtitle->Text =
                L"Review controlled requests for emergency changes to historical academic records.";

            this->lblSubtitle->Dock = DockStyle::Top;
            this->lblSubtitle->Height = 22;
            this->lblSubtitle->ForeColor =
                Color::FromArgb(71, 85, 105);

            header->Controls->Add(this->lblSubtitle);
            header->Controls->Add(this->lblTitle);

            Panel^ toolbar =
                gcnew Panel();

            toolbar->Dock = DockStyle::Fill;
            toolbar->BackColor = Color::White;

            this->btnRefresh =
                gcnew Button();
            this->btnRefresh->Text = L"Refresh";
            this->btnRefresh->Width = 100;
            this->btnRefresh->Height = 34;
            this->btnRefresh->Location = Drawing::Point(10, 8);

            this->btnApprove =
                gcnew Button();
            this->btnApprove->Text = L"Approve";
            this->btnApprove->Width = 105;
            this->btnApprove->Height = 34;
            this->btnApprove->Location = Drawing::Point(118, 8);

            this->btnReject =
                gcnew Button();
            this->btnReject->Text = L"Reject";
            this->btnReject->Width = 105;
            this->btnReject->Height = 34;
            this->btnReject->Location = Drawing::Point(231, 8);

            this->btnRefresh->Click +=
                gcnew EventHandler(
                    this,
                    &AcademicEditApprovals::RefreshClicked
                );

            this->btnApprove->Click +=
                gcnew EventHandler(
                    this,
                    &AcademicEditApprovals::ApproveClicked
                );

            this->btnReject->Click +=
                gcnew EventHandler(
                    this,
                    &AcademicEditApprovals::RejectClicked
                );

            toolbar->Controls->Add(this->btnRefresh);
            toolbar->Controls->Add(this->btnApprove);
            toolbar->Controls->Add(this->btnReject);

            Panel^ gridPanel =
                gcnew Panel();

            gridPanel->Dock = DockStyle::Fill;
            gridPanel->Padding =
                System::Windows::Forms::Padding(0, 8, 0, 8);

            ConfigureGrid();

            gridPanel->Controls->Add(
                this->approvalsGrid
            );

            Panel^ footer =
                gcnew Panel();

            footer->Dock = DockStyle::Fill;

            this->lblCount =
                gcnew Label();

            this->lblCount->Text =
                L"Requests: 0";

            this->lblCount->Dock =
                DockStyle::Left;

            this->lblCount->Width = 200;
            this->lblCount->TextAlign =
                ContentAlignment::MiddleLeft;

            this->btnClose =
                gcnew Button();

            this->btnClose->Text =
                L"Close";

            this->btnClose->Width = 100;
            this->btnClose->Height = 34;
            this->btnClose->Dock =
                DockStyle::Right;

            this->btnClose->Click +=
                gcnew EventHandler(
                    this,
                    &AcademicEditApprovals::CloseClicked
                );

            footer->Controls->Add(
                this->btnClose
            );

            footer->Controls->Add(
                this->lblCount
            );

            root->Controls->Add(header, 0, 0);
            root->Controls->Add(toolbar, 0, 1);
            root->Controls->Add(gridPanel, 0, 2);
            root->Controls->Add(footer, 0, 3);

            this->Controls->Add(root);

            this->ResumeLayout(false);
        }

    public:
        AcademicEditApprovals(void)
        {
            InitializeComponent();

            ThemeManager::ApplyToForm(this);

            if (!AuthSession::HasPermission(
                    L"academic_records.override_inactive_year"))
            {
                MessageBox::Show(
                    L"You do not have permission to access Academic Edit Approvals.",
                    L"Access Denied",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                this->Shown +=
                    gcnew EventHandler(
                        this,
                        &AcademicEditApprovals::CloseUnauthorized
                    );

                return;
            }

            if (System::ComponentModel::LicenseManager::UsageMode
                != System::ComponentModel::LicenseUsageMode::Designtime)
            {
                LoadApprovals();
            }
        }

        Void CloseUnauthorized(
            Object^,
            EventArgs^)
        {
            this->Close();
        }
    };
}
