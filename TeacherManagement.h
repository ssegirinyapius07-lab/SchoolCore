#pragma once

#include "DbConnection.h"

#include <mariadb/conncpp.hpp>
#include <msclr/marshal_cppstd.h>

#include <memory>
#include <string>
#include <stdexcept>

using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;

namespace SchoolCore
{
    public ref class TeacherManagement : public Form
    {
    private:
        // =========================================================
        // MAIN FORM
        // =========================================================

        TableLayoutPanel^ mainLayout;
        Panel^ headerPanel;
        Label^ lblTitle;
        Label^ lblSubtitle;

        Panel^ actionPanel;
        Button^ btnRegisterTeacher;
        Label^ lblSearch;
        TextBox^ txtSearch;
        Button^ btnSearch;

        Label^ lblTeacherCount;
        DataGridView^ teachersGrid;

        FlowLayoutPanel^ buttonPanel;
        Button^ btnViewProfile;
        Button^ btnEditTeacher;
        Button^ btnToggleTeacher;
        Button^ btnBack;

        // =========================================================
        // EDITOR DIALOG
        // =========================================================

        Form^ editorForm;
        TextBox^ txtStaffNumber;
        TextBox^ txtFirstName;
        TextBox^ txtMiddleName;
        TextBox^ txtLastName;
        ComboBox^ cmbGender;
        TextBox^ txtPhoneNumber;
        TextBox^ txtEmail;
        TextBox^ txtAddress;
        Button^ btnEditorSave;
        Button^ btnEditorCancel;

        bool editorEditMode = false;
        long long editingTeacherId = 0;


        // =========================================================
        // PROFILE HELPER
        // =========================================================

        void AddProfileField(
            TableLayoutPanel^ layout,
            int row,
            String^ labelText,
            String^ valueText)
        {
            Label^ label = gcnew Label();

            label->Text = labelText;
            label->Dock = DockStyle::Fill;
            label->Font = gcnew System::Drawing::Font(
                L"Segoe UI",
                9.5F,
                FontStyle::Bold
            );
            label->ForeColor = Color::FromArgb(71, 85, 105);
            label->TextAlign = ContentAlignment::MiddleLeft;
            label->Margin =
                System::Windows::Forms::Padding(0, 3, 12, 3);


            Label^ value = gcnew Label();

            value->Text =
                String::IsNullOrWhiteSpace(valueText)
                ? L"Not provided"
                : valueText;

            value->Dock = DockStyle::Fill;
            value->Font = gcnew System::Drawing::Font(
                L"Segoe UI",
                9.5F,
                FontStyle::Regular
            );
            value->ForeColor = Color::FromArgb(30, 41, 59);
            value->Margin =
                System::Windows::Forms::Padding(0, 3, 0, 3);
            value->TextAlign = ContentAlignment::MiddleLeft;

            layout->Controls->Add(label, 0, row);
            layout->Controls->Add(value, 1, row);
        }


        // =========================================================
        // LOAD TEACHERS
        // =========================================================

        void LoadTeachers()
        {
            try
            {
                auto con = DbConnection::GetConnection();

                String^ searchText =
                    this->txtSearch->Text->Trim();

                String^ patternText =
                    L"%" + searchText + L"%";

                std::unique_ptr<sql::PreparedStatement> stmt;

                if (String::IsNullOrWhiteSpace(searchText))
                {
                    stmt.reset(
                        con->prepareStatement(
                            "SELECT "
                            "teacher_id, "
                            "staff_number, "
                            "first_name, "
                            "middle_name, "
                            "last_name, "
                            "gender, "
                            "phone_number, "
                            "email, "
                            "employment_status "
                            "FROM teachers "
                            "ORDER BY teacher_id DESC"
                        )
                    );
                }
                else
                {
                    stmt.reset(
                        con->prepareStatement(
                            "SELECT "
                            "teacher_id, "
                            "staff_number, "
                            "first_name, "
                            "middle_name, "
                            "last_name, "
                            "gender, "
                            "phone_number, "
                            "email, "
                            "employment_status "
                            "FROM teachers "
                            "WHERE staff_number LIKE ? "
                            "OR first_name LIKE ? "
                            "OR middle_name LIKE ? "
                            "OR last_name LIKE ? "
                            "OR CONCAT_WS(' ', first_name, middle_name, last_name) LIKE ? "
                            "OR phone_number LIKE ? "
                            "OR email LIKE ? "
                            "ORDER BY teacher_id DESC"
                        )
                    );

                    std::string pattern =
                        msclr::interop::marshal_as<std::string>(
                            patternText
                        );

                    for (int i = 1; i <= 7; i++)
                    {
                        stmt->setString(i, pattern);
                    }
                }

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                this->teachersGrid->Rows->Clear();

                int teacherCount = 0;

                while (result->next())
                {
                    String^ middleName = L"";

                    if (!result->isNull("middle_name"))
                    {
                        middleName =
                            gcnew String(
                                result->getString(
                                    "middle_name"
                                ).c_str()
                            );
                    }

                    String^ fullName =
                        gcnew String(
                            result->getString(
                                "first_name"
                            ).c_str()
                        ) +
                        L" ";

                    if (!String::IsNullOrWhiteSpace(middleName))
                    {
                        fullName += middleName + L" ";
                    }

                    fullName +=
                        gcnew String(
                            result->getString(
                                "last_name"
                            ).c_str()
                        );

                    String^ gender =
                        result->isNull("gender")
                        ? L"Not provided"
                        : gcnew String(
                            result->getString(
                                "gender"
                            ).c_str()
                        );

                    String^ phone =
                        result->isNull("phone_number")
                        ? L"Not provided"
                        : gcnew String(
                            result->getString(
                                "phone_number"
                            ).c_str()
                        );

                    String^ status =
                        gcnew String(
                            result->getString(
                                "employment_status"
                            ).c_str()
                        );

                    this->teachersGrid->Rows->Add(
                        result->getInt64("teacher_id"),
                        gcnew String(
                            result->getString(
                                "staff_number"
                            ).c_str()
                        ),
                        fullName,
                        gender,
                        phone,
                        status
                    );

                    teacherCount++;
                }

                this->lblTeacherCount->Text =
                    (String::IsNullOrWhiteSpace(searchText)
                    ? L"Teachers: "
                    : L"Matching Teachers: ") +
                    teacherCount.ToString();

                bool hasSelection =
                    this->teachersGrid->SelectedRows->Count > 0;

                this->btnViewProfile->Enabled = hasSelection;
                this->btnEditTeacher->Enabled = hasSelection;
                this->btnToggleTeacher->Enabled = hasSelection;
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }


        // =========================================================
        // SEARCH
        // =========================================================

        System::Void btnSearch_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            LoadTeachers();
        }


        System::Void txtSearch_KeyDown(
            System::Object^ sender,
            KeyEventArgs^ e)
        {
            if (e->KeyCode == Keys::Enter)
            {
                LoadTeachers();
                e->SuppressKeyPress = true;
            }
        }


        // =========================================================
        // SELECTION
        // =========================================================

        System::Void teachersGrid_SelectionChanged(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            bool hasSelection =
                this->teachersGrid->SelectedRows->Count > 0;

            this->btnViewProfile->Enabled = hasSelection;
            this->btnEditTeacher->Enabled = hasSelection;
            this->btnToggleTeacher->Enabled = hasSelection;
        }


        // =========================================================
        // EDITOR HELPERS
        // =========================================================

        void AddEditorLabel(
            TableLayoutPanel^ layout,
            String^ text,
            int row)
        {
            Label^ label =
                gcnew Label();

            label->Text = text;
            label->Dock = DockStyle::Fill;
            label->TextAlign =
                ContentAlignment::MiddleLeft;
            label->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F,
                    FontStyle::Bold
                );
            label->ForeColor =
                Color::FromArgb(
                    71, 85, 105
                );

            layout->Controls->Add(
                label,
                0,
                row
            );
        }


        void AddEditorTextBox(
            TableLayoutPanel^ layout,
            TextBox^ box,
            int row)
        {
            box->Dock =
                DockStyle::Fill;

            box->Margin =
                System::Windows::Forms::Padding(
                    0,
                    5,
                    0,
                    5
                );

            layout->Controls->Add(
                box,
                1,
                row
            );
        }


        // =========================================================
        // OPEN EDITOR
        // =========================================================

        void OpenTeacherEditor(
            long long teacherId,
            bool editMode)
        {
            this->editorEditMode = editMode;
            this->editingTeacherId = teacherId;

            this->editorForm = gcnew Form();

            this->editorForm->Text =
                editMode
                ? L"Edit Teacher"
                : L"Register Teacher";

            this->editorForm->StartPosition =
                FormStartPosition::CenterParent;

            this->editorForm->FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::FixedSingle;

            this->editorForm->MaximizeBox = false;
            this->editorForm->MinimizeBox = false;
            this->editorForm->ShowInTaskbar = false;
            this->editorForm->ClientSize =
                System::Drawing::Size(650, 610);

            this->editorForm->BackColor =
                Color::FromArgb(248, 250, 252);


            Panel^ header =
                gcnew Panel();

            header->Dock = DockStyle::Top;
            header->Height = 90;
            header->BackColor =
                Color::FromArgb(30, 41, 59);
            header->Padding =
                System::Windows::Forms::Padding(20, 12, 20, 10);


            Label^ title =
                gcnew Label();

            title->Text =
                editMode
                ? L"Edit Teacher"
                : L"Register Teacher";

            title->Dock = DockStyle::Top;
            title->Height = 36;
            title->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI Semibold",
                    18.0F,
                    FontStyle::Bold
                );
            title->ForeColor = Color::White;


            Label^ subtitle =
                gcnew Label();

            subtitle->Text =
                editMode
                ? L"Update the teacher's staff and contact information."
                : L"Enter the teacher's details. Staff number is generated automatically.";

            subtitle->Dock = DockStyle::Fill;
            subtitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F
                );
            subtitle->ForeColor =
                Color::Gainsboro;


            header->Controls->Add(subtitle);
            header->Controls->Add(title);


            TableLayoutPanel^ layout =
                gcnew TableLayoutPanel();

            layout->Dock = DockStyle::Fill;
            layout->Padding =
                System::Windows::Forms::Padding(24, 20, 24, 15);
            layout->ColumnCount = 2;
            layout->RowCount = 8;

            layout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Absolute,
                    145.0F
                )
            );

            layout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    100.0F
                )
            );

            for (int i = 0; i < 7; i++)
            {
                layout->RowStyles->Add(
                    gcnew RowStyle(
                        SizeType::Absolute,
                        42.0F
                    )
                );
            }

            layout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Percent,
                    100.0F
                )
            );




            this->txtStaffNumber = gcnew TextBox();
            this->txtStaffNumber->ReadOnly = true;
            this->txtStaffNumber->BackColor = Color::WhiteSmoke;
            this->txtFirstName = gcnew TextBox();
            this->txtMiddleName = gcnew TextBox();
            this->txtLastName = gcnew TextBox();
            this->cmbGender = gcnew ComboBox();
            this->txtPhoneNumber = gcnew TextBox();
            this->txtEmail = gcnew TextBox();
            this->txtAddress = gcnew TextBox();


            AddEditorLabel(layout, L"Staff Number", 0);
            AddEditorTextBox(layout, this->txtStaffNumber, 0);

            AddEditorLabel(layout, L"First Name", 1);
            AddEditorTextBox(layout, this->txtFirstName, 1);

            AddEditorLabel(layout, L"Middle Name", 2);
            AddEditorTextBox(layout, this->txtMiddleName, 2);

            AddEditorLabel(layout, L"Last Name", 3);
            AddEditorTextBox(layout, this->txtLastName, 3);

            AddEditorLabel(layout, L"Gender", 4);

            this->cmbGender->Dock = DockStyle::Fill;
            this->cmbGender->Margin =
                System::Windows::Forms::Padding(
                    0, 5, 0, 5
                );
            this->cmbGender->DropDownStyle =
                ComboBoxStyle::DropDownList;

            this->cmbGender->Items->Add(
                L"Select Gender"
            );
            this->cmbGender->Items->Add(
                L"Male"
            );
            this->cmbGender->Items->Add(
                L"Female"
            );

            this->cmbGender->SelectedIndex = 0;

            layout->Controls->Add(
                this->cmbGender,
                1,
                4
            );

            AddEditorLabel(layout, L"Phone Number", 5);
            AddEditorTextBox(layout, this->txtPhoneNumber, 5);

            AddEditorLabel(layout, L"Email", 6);
            AddEditorTextBox(layout, this->txtEmail, 6);


            AddEditorLabel(layout, L"Address", 7);

            this->txtAddress->Multiline = true;
            this->txtAddress->ScrollBars =
                ScrollBars::Vertical;
            this->txtAddress->Dock = DockStyle::Fill;
            this->txtAddress->Margin =
                System::Windows::Forms::Padding(
                    0, 5, 0, 5
                );

            layout->Controls->Add(
                this->txtAddress,
                1,
                7
            );


            Panel^ footer =
                gcnew Panel();

            footer->Dock = DockStyle::Bottom;
            footer->Height = 65;
            footer->Padding =
                System::Windows::Forms::Padding(
                    24,
                    10,
                    24,
                    10
                );


            this->btnEditorCancel =
                gcnew Button();

            this->btnEditorCancel->Text =
                L"Cancel";

            this->btnEditorCancel->Width = 110;
            this->btnEditorCancel->Height = 40;
            this->btnEditorCancel->Dock =
                DockStyle::Right;


            this->btnEditorSave =
                gcnew Button();

            this->btnEditorSave->Text =
                editMode
                ? L"Update Teacher"
                : L"Save Teacher";

            this->btnEditorSave->Width = 140;
            this->btnEditorSave->Height = 40;
            this->btnEditorSave->Dock =
                DockStyle::Right;
            this->btnEditorSave->Margin =
                System::Windows::Forms::Padding(
                    0,
                    0,
                    10,
                    0
                );

            this->btnEditorSave->BackColor =
                Color::FromArgb(
                    38, 117, 92
                );

            this->btnEditorSave->ForeColor =
                Color::White;

            this->btnEditorSave->FlatStyle =
                FlatStyle::Flat;

            this->btnEditorSave->FlatAppearance->BorderSize = 0;


            footer->Controls->Add(
                this->btnEditorCancel
            );

            footer->Controls->Add(
                this->btnEditorSave
            );


            this->btnEditorCancel->Click +=
                gcnew System::EventHandler(
                    this,
                    &TeacherManagement::btnEditorCancel_Click
                );

            this->btnEditorSave->Click +=
                gcnew System::EventHandler(
                    this,
                    &TeacherManagement::btnEditorSave_Click
                );


            this->txtStaffNumber->Text =
                editMode
                ? this->txtStaffNumber->Text
                : L"Generated automatically";

            this->editorForm->Controls->Add(layout);
            this->editorForm->Controls->Add(footer);
            this->editorForm->Controls->Add(header);


            if (editMode)
            {
                LoadTeacherForEdit(teacherId);
            }

            this->editorForm->ShowDialog(this);
        }


        // =========================================================
        // LOAD TEACHER FOR EDIT
        // =========================================================

        void LoadTeacherForEdit(
            long long teacherId)
        {
            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "staff_number, "
                        "first_name, "
                        "middle_name, "
                        "last_name, "
                        "gender, "
                        "phone_number, "
                        "email, "
                        "address "
                        "FROM teachers "
                        "WHERE teacher_id = ? "
                        "LIMIT 1"
                    )
                );

                stmt->setInt64(
                    1,
                    teacherId
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                if (!result->next())
                {
                    MessageBox::Show(
                        L"Teacher record could not be found.",
                        L"Edit Teacher",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning
                    );

                    this->editorForm->Close();
                    return;
                }

                this->txtStaffNumber->Text =
                    gcnew String(
                        result->getString(
                            "staff_number"
                        ).c_str()
                    );

                this->txtFirstName->Text =
                    gcnew String(
                        result->getString(
                            "first_name"
                        ).c_str()
                    );

                if (!result->isNull("middle_name"))
                {
                    this->txtMiddleName->Text =
                        gcnew String(
                            result->getString(
                                "middle_name"
                            ).c_str()
                        );
                }

                this->txtLastName->Text =
                    gcnew String(
                        result->getString(
                            "last_name"
                        ).c_str()
                    );

                if (!result->isNull("gender"))
                {
                    String^ gender =
                        gcnew String(
                            result->getString(
                                "gender"
                            ).c_str()
                        );

                    for (int i = 0;
                        i < this->cmbGender->Items->Count;
                        i++)
                    {
                        if (
                            Convert::ToString(
                                this->cmbGender->Items[i]
                            )->Equals(
                                gender,
                                StringComparison::OrdinalIgnoreCase
                            )
                        )
                        {
                            this->cmbGender->SelectedIndex = i;
                            break;
                        }
                    }
                }

                if (!result->isNull("phone_number"))
                {
                    this->txtPhoneNumber->Text =
                        gcnew String(
                            result->getString(
                                "phone_number"
                            ).c_str()
                        );
                }

                if (!result->isNull("email"))
                {
                    this->txtEmail->Text =
                        gcnew String(
                            result->getString(
                                "email"
                            ).c_str()
                        );
                }

                if (!result->isNull("address"))
                {
                    this->txtAddress->Text =
                        gcnew String(
                            result->getString(
                                "address"
                            ).c_str()
                        );
                }
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );

                this->editorForm->Close();
            }
        }


        // =========================================================
        // SAVE / UPDATE TEACHER
        // =========================================================

        System::Void btnEditorSave_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            if (
                String::IsNullOrWhiteSpace(
                    this->txtFirstName->Text
                )
            )
            {
                MessageBox::Show(
                    L"Please enter the teacher's first name.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                this->txtFirstName->Focus();
                return;
            }

            if (
                String::IsNullOrWhiteSpace(
                    this->txtLastName->Text
                )
            )
            {
                MessageBox::Show(
                    L"Please enter the teacher's last name.",
                    L"Validation",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                this->txtLastName->Focus();
                return;
            }

            std::unique_ptr<sql::Connection> con;

            try
            {
                con =
                    DbConnection::GetConnection();

                std::string firstName =
                    msclr::interop::marshal_as<std::string>(
                        this->txtFirstName->Text->Trim()
                    );

                std::string middleName =
                    msclr::interop::marshal_as<std::string>(
                        this->txtMiddleName->Text->Trim()
                    );

                std::string lastName =
                    msclr::interop::marshal_as<std::string>(
                        this->txtLastName->Text->Trim()
                    );

                std::string gender =
                    this->cmbGender->SelectedIndex > 0
                    ? msclr::interop::marshal_as<std::string>(
                        this->cmbGender->SelectedItem->ToString()
                    )
                    : "";

                std::string phone =
                    msclr::interop::marshal_as<std::string>(
                        this->txtPhoneNumber->Text->Trim()
                    );

                std::string email =
                    msclr::interop::marshal_as<std::string>(
                        this->txtEmail->Text->Trim()
                    );

                std::string address =
                    msclr::interop::marshal_as<std::string>(
                        this->txtAddress->Text->Trim()
                    );


                if (editorEditMode)
                {
                    std::unique_ptr<sql::PreparedStatement>
                        stmt(
                            con->prepareStatement(
                                "UPDATE teachers "
                                "SET first_name = ?, "
                                "middle_name = ?, "
                                "last_name = ?, "
                                "gender = ?, "
                                "phone_number = ?, "
                                "email = ?, "
                                "address = ? "
                                "WHERE teacher_id = ?"
                            )
                        );

                    stmt->setString(1, firstName);
                    stmt->setString(2, middleName);
                    stmt->setString(3, lastName);
                    stmt->setString(4, gender);
                    stmt->setString(5, phone);
                    stmt->setString(6, email);
                    stmt->setString(7, address);
                    stmt->setInt64(8, this->editingTeacherId);

                    stmt->executeUpdate();

                    MessageBox::Show(
                        L"Teacher details updated successfully.",
                        L"Teachers",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Information
                    );
                }
                else
                {
                    // -------------------------------------------------
                    // GENERATE STAFF NUMBER IN A DATABASE TRANSACTION
                    // -------------------------------------------------

                    con->setAutoCommit(false);

                    int staffYear =
                        DateTime::Now.Year;

                    std::string yearPattern =
                        "TCH/" +
                        std::to_string(staffYear) +
                        "/%";

                    // Find the highest staff sequence already stored
                    // for this year so existing records are not repeated.
                    std::unique_ptr<sql::PreparedStatement>
                        maxStmt(
                            con->prepareStatement(
                                "SELECT "
                                "COALESCE("
                                "MAX("
                                "CAST("
                                "SUBSTRING_INDEX(staff_number, '/', -1) "
                                "AS UNSIGNED)"
                                "), "
                                "0"
                                ") AS max_sequence "
                                "FROM teachers "
                                "WHERE staff_number LIKE ?"
                            )
                        );

                    maxStmt->setString(
                        1,
                        yearPattern
                    );

                    std::unique_ptr<sql::ResultSet>
                        maxResult(
                            maxStmt->executeQuery()
                        );

                    int maxExistingSequence = 0;

                    if (maxResult->next())
                    {
                        maxExistingSequence =
                            maxResult->getInt(
                                "max_sequence"
                            );
                    }

                    // Keep the sequence in its own database table.
                    // The row is locked/updated inside this transaction.
                    std::unique_ptr<sql::PreparedStatement>
                        sequenceInit(
                            con->prepareStatement(
                                "INSERT INTO "
                                "teacher_staff_number_sequences "
                                "(staff_year, last_sequence) "
                                "VALUES (?, ?) "
                                "ON DUPLICATE KEY UPDATE "
                                "last_sequence = "
                                "GREATEST("
                                "last_sequence, "
                                "VALUES(last_sequence)"
                                ")"
                            )
                        );

                    sequenceInit->setInt(
                        1,
                        staffYear
                    );

                    sequenceInit->setInt(
                        2,
                        maxExistingSequence
                    );

                    sequenceInit->executeUpdate();


                    std::unique_ptr<sql::PreparedStatement>
                        sequenceUpdate(
                            con->prepareStatement(
                                "UPDATE "
                                "teacher_staff_number_sequences "
                                "SET last_sequence = last_sequence + 1 "
                                "WHERE staff_year = ?"
                            )
                        );

                    sequenceUpdate->setInt(
                        1,
                        staffYear
                    );

                    sequenceUpdate->executeUpdate();


                    std::unique_ptr<sql::PreparedStatement>
                        sequenceSelect(
                            con->prepareStatement(
                                "SELECT last_sequence "
                                "FROM teacher_staff_number_sequences "
                                "WHERE staff_year = ? "
                                "FOR UPDATE"
                            )
                        );

                    sequenceSelect->setInt(
                        1,
                        staffYear
                    );

                    std::unique_ptr<sql::ResultSet>
                        sequenceResult(
                            sequenceSelect->executeQuery()
                        );

                    if (!sequenceResult->next())
                    {
                        throw std::runtime_error(
                            "Unable to generate teacher staff number."
                        );
                    }

                    int sequenceNumber =
                        sequenceResult->getInt(
                            "last_sequence"
                        );

                    String^ generatedStaffNumber =
                        String::Format(
                            L"TCH/{0}/{1:D4}",
                            staffYear,
                            sequenceNumber
                        );

                    std::string staffNumber =
                        msclr::interop::marshal_as<std::string>(
                            generatedStaffNumber
                        );


                    // -------------------------------------------------
                    // INSERT TEACHER
                    // -------------------------------------------------

                    std::unique_ptr<sql::PreparedStatement>
                        stmt(
                            con->prepareStatement(
                                "INSERT INTO teachers "
                                "("
                                "staff_number, "
                                "first_name, "
                                "middle_name, "
                                "last_name, "
                                "gender, "
                                "phone_number, "
                                "email, "
                                "address, "
                                "employment_status"
                                ") "
                                "VALUES (?, ?, ?, ?, ?, ?, ?, ?, 'Active')"
                            )
                        );

                    stmt->setString(
                        1,
                        staffNumber
                    );
                    stmt->setString(2, firstName);
                    stmt->setString(3, middleName);
                    stmt->setString(4, lastName);
                    stmt->setString(5, gender);
                    stmt->setString(6, phone);
                    stmt->setString(7, email);
                    stmt->setString(8, address);

                    stmt->executeUpdate();

                    con->commit();

                    MessageBox::Show(
                        L"Teacher registered successfully.\n\n"
                        L"Staff Number: " +
                        generatedStaffNumber,
                        L"Teachers",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Information
                    );
                }

                this->editorForm->DialogResult =
                    System::Windows::Forms::DialogResult::OK;

                this->editorForm->Close();
            }
            catch (sql::SQLException& ex)
            {
                if (con && !editorEditMode)
                {
                    try
                    {
                        con->rollback();
                    }
                    catch (...)
                    {
                    }
                }

                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
            catch (std::exception& ex)
            {
                if (con && !editorEditMode)
                {
                    try
                    {
                        con->rollback();
                    }
                    catch (...)
                    {
                    }
                }

                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }


        System::Void btnEditorCancel_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            this->editorForm->Close();
        }


        // =========================================================
        // REGISTER TEACHER
        // =========================================================

        System::Void btnRegisterTeacher_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            OpenTeacherEditor(0, false);

            if (this->editorForm->DialogResult ==
                System::Windows::Forms::DialogResult::OK)
            {
                LoadTeachers();
            }
        }


        // =========================================================
        // VIEW PROFILE
        // =========================================================

        System::Void btnViewProfile_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            if (this->teachersGrid->SelectedRows->Count == 0)
            {
                MessageBox::Show(
                    L"Please select a teacher.",
                    L"Teachers",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                return;
            }

            long long teacherId =
                Convert::ToInt64(
                    this->teachersGrid
                    ->SelectedRows[0]
                    ->Cells["TeacherId"]
                    ->Value
                );

            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement> stmt(
                    con->prepareStatement(
                        "SELECT "
                        "staff_number, "
                        "first_name, "
                        "middle_name, "
                        "last_name, "
                        "gender, "
                        "phone_number, "
                        "email, "
                        "address, "
                        "employment_status, "
                        "created_at "
                        "FROM teachers "
                        "WHERE teacher_id = ? "
                        "LIMIT 1"
                    )
                );

                stmt->setInt64(
                    1,
                    teacherId
                );

                std::unique_ptr<sql::ResultSet> result(
                    stmt->executeQuery()
                );

                if (!result->next())
                {
                    MessageBox::Show(
                        L"Teacher record could not be found.",
                        L"Teachers",
                        MessageBoxButtons::OK,
                        MessageBoxIcon::Warning
                    );

                    return;
                }

                String^ middleName = L"";

                if (!result->isNull("middle_name"))
                {
                    middleName =
                        gcnew String(
                            result->getString(
                                "middle_name"
                            ).c_str()
                        );
                }

                String^ fullName =
                    gcnew String(
                        result->getString(
                            "first_name"
                        ).c_str()
                    ) +
                    L" ";

                if (!String::IsNullOrWhiteSpace(middleName))
                {
                    fullName += middleName + L" ";
                }

                fullName +=
                    gcnew String(
                        result->getString(
                            "last_name"
                        ).c_str()
                    );

                String^ gender =
                    result->isNull("gender")
                    ? L""
                    : gcnew String(
                        result->getString(
                            "gender"
                        ).c_str()
                    );

                String^ phone =
                    result->isNull("phone_number")
                    ? L""
                    : gcnew String(
                        result->getString(
                            "phone_number"
                        ).c_str()
                    );

                String^ email =
                    result->isNull("email")
                    ? L""
                    : gcnew String(
                        result->getString(
                            "email"
                        ).c_str()
                    );

                String^ address =
                    result->isNull("address")
                    ? L""
                    : gcnew String(
                        result->getString(
                            "address"
                        ).c_str()
                    );

                String^ status =
                    gcnew String(
                        result->getString(
                            "employment_status"
                        ).c_str()
                    );

                String^ createdAt =
                    result->isNull("created_at")
                    ? L"Not recorded"
                    : gcnew String(
                        result->getString(
                            "created_at"
                        ).c_str()
                    );


                Form^ profileForm = gcnew Form();

                profileForm->Text =
                    L"Teacher Profile";

                profileForm->StartPosition =
                    FormStartPosition::CenterScreen;

                profileForm->FormBorderStyle =
                    System::Windows::Forms::FormBorderStyle::FixedSingle;

                profileForm->MaximizeBox = false;
                profileForm->MinimizeBox = false;
                profileForm->ShowInTaskbar = false;
                profileForm->ClientSize =
                    System::Drawing::Size(720, 700);

                profileForm->BackColor =
                    Color::FromArgb(
                        248, 250, 252
                    );


                Panel^ profileHeader =
                    gcnew Panel();

                profileHeader->Dock =
                    DockStyle::Top;

                profileHeader->Height =
                    140;

                profileHeader->BackColor =
                    Color::FromArgb(
                        30, 41, 59
                    );

                profileHeader->Padding =
                    System::Windows::Forms::Padding(
                        24,
                        16,
                        24,
                        12
                    );


                Label^ nameLabel =
                    gcnew Label();

                nameLabel->Text =
                    fullName;

                nameLabel->Dock =
                    DockStyle::Top;

                nameLabel->Height =
                    46;

                nameLabel->Font =
                    gcnew System::Drawing::Font(
                        L"Segoe UI Semibold",
                        20.0F,
                        FontStyle::Bold
                    );

                nameLabel->ForeColor =
                    Color::White;

                nameLabel->TextAlign =
                    ContentAlignment::MiddleLeft;


                Label^ staffLabel =
                    gcnew Label();

                staffLabel->Text =
                    L"Staff No.  " +
                    gcnew String(
                        result->getString(
                            "staff_number"
                        ).c_str()
                    );

                staffLabel->Dock =
                    DockStyle::Top;

                staffLabel->Height =
                    30;

                staffLabel->Font =
                    gcnew System::Drawing::Font(
                        L"Segoe UI",
                        10.0F
                    );

                staffLabel->ForeColor =
                    Color::Gainsboro;


                Label^ statusLabel =
                    gcnew Label();

                statusLabel->Text =
                    L"Status: " +
                    status;

                statusLabel->Dock =
                    DockStyle::Top;

                statusLabel->Height =
                    30;

                statusLabel->Font =
                    gcnew System::Drawing::Font(
                        L"Segoe UI Semibold",
                        9.0F,
                        FontStyle::Bold
                    );

                statusLabel->ForeColor =
                    status->Equals(
                        L"Active",
                        StringComparison::OrdinalIgnoreCase
                    )
                    ? Color::FromArgb(167, 243, 208)
                    : Color::FromArgb(254, 202, 202);


                profileHeader->Controls->Add(statusLabel);
                profileHeader->Controls->Add(staffLabel);
                profileHeader->Controls->Add(nameLabel);


                Panel^ content =
                    gcnew Panel();

                content->Dock =
                    DockStyle::Fill;

                content->AutoScroll =
                    true;

                content->Padding =
                    System::Windows::Forms::Padding(
                        20
                    );


                TableLayoutPanel^ layout =
                    gcnew TableLayoutPanel();

                layout->Dock =
                    DockStyle::Top;

                layout->AutoSize =
                    true;

                layout->ColumnCount =
                    1;

                layout->RowCount =
                    2;

                layout->ColumnStyles->Add(
                    gcnew ColumnStyle(
                        SizeType::Percent,
                        100.0F
                    )
                );


                GroupBox^ info =
                    gcnew GroupBox();

                info->Text =
                    L"Teacher Information";

                info->Dock =
                    DockStyle::Top;

                info->Height =
                    400;

                info->Padding =
                    System::Windows::Forms::Padding(
                        14,
                        18,
                        14,
                        10
                    );

                info->Margin =
                    System::Windows::Forms::Padding(
                        0,
                        0,
                        0,
                        16
                    );


                TableLayoutPanel^ infoLayout =
                    gcnew TableLayoutPanel();

                infoLayout->Dock =
                    DockStyle::Fill;

                infoLayout->ColumnCount =
                    2;

                infoLayout->RowCount =
                    9;

                infoLayout->ColumnStyles->Add(
                    gcnew ColumnStyle(
                        SizeType::Absolute,
                        150.0F
                    )
                );

                infoLayout->ColumnStyles->Add(
                    gcnew ColumnStyle(
                        SizeType::Percent,
                        100.0F
                    )
                );

                for (int i = 0; i < 9; i++)
                {
                    float rowHeight =
                        (i == 5)
                        ? 48.0F
                        : 36.0F;

                    infoLayout->RowStyles->Add(
                        gcnew RowStyle(
                            SizeType::Absolute,
                            rowHeight
                        )
                    );
                }


                AddProfileField(
                    infoLayout,
                    0,
                    L"Staff Number",
                    gcnew String(
                        result->getString(
                            "staff_number"
                        ).c_str()
                    )
                );

                AddProfileField(
                    infoLayout,
                    1,
                    L"Full Name",
                    fullName
                );

                AddProfileField(
                    infoLayout,
                    2,
                    L"Gender",
                    gender
                );

                AddProfileField(
                    infoLayout,
                    3,
                    L"Phone Number",
                    phone
                );

                AddProfileField(
                    infoLayout,
                    4,
                    L"Email",
                    email
                );

                AddProfileField(
                    infoLayout,
                    5,
                    L"Address",
                    address
                );

                AddProfileField(
                    infoLayout,
                    6,
                    L"Employment Status",
                    status
                );

                AddProfileField(
                    infoLayout,
                    7,
                    L"Created",
                    createdAt
                );

                AddProfileField(
                    infoLayout,
                    8,
                    L"Teacher ID",
                    Convert::ToString(teacherId)
                );


                info->Controls->Add(
                    infoLayout
                );

                layout->Controls->Add(
                    info,
                    0,
                    0
                );


                content->Controls->Add(
                    layout
                );


                Panel^ footer =
                    gcnew Panel();

                footer->Dock =
                    DockStyle::Bottom;

                footer->Height =
                    60;

                footer->Padding =
                    System::Windows::Forms::Padding(
                        20,
                        8,
                        20,
                        10
                    );


                Button^ closeButton =
                    gcnew Button();

                closeButton->Text =
                    L"Close";

                closeButton->Dock =
                    DockStyle::Right;

                closeButton->Width =
                    110;

                closeButton->Height =
                    38;

                closeButton->BackColor =
                    Color::FromArgb(
                        30, 41, 59
                    );

                closeButton->ForeColor =
                    Color::White;

                closeButton->FlatStyle =
                    FlatStyle::Flat;

                closeButton->FlatAppearance->BorderSize =
                    0;

                closeButton->DialogResult =
                    System::Windows::Forms::DialogResult::Cancel;


                footer->Controls->Add(
                    closeButton
                );


                profileForm->Controls->Add(content);
                profileForm->Controls->Add(footer);
                profileForm->Controls->Add(profileHeader);

                profileForm->ShowDialog(this);
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }


        // =========================================================
        // EDIT
        // =========================================================

        System::Void btnEditTeacher_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            if (this->teachersGrid->SelectedRows->Count == 0)
            {
                MessageBox::Show(
                    L"Please select a teacher.",
                    L"Teachers",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                return;
            }

            long long teacherId =
                Convert::ToInt64(
                    this->teachersGrid
                    ->SelectedRows[0]
                    ->Cells["TeacherId"]
                    ->Value
                );

            OpenTeacherEditor(
                teacherId,
                true
            );

            if (this->editorForm->DialogResult ==
                System::Windows::Forms::DialogResult::OK)
            {
                LoadTeachers();
            }
        }


        // =========================================================
        // TOGGLE STATUS
        // =========================================================

        System::Void btnToggleTeacher_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            if (this->teachersGrid->SelectedRows->Count == 0)
            {
                MessageBox::Show(
                    L"Please select a teacher.",
                    L"Teachers",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Warning
                );

                return;
            }

            DataGridViewRow^ row =
                this->teachersGrid
                ->SelectedRows[0];

            long long teacherId =
                Convert::ToInt64(
                    row->Cells["TeacherId"]->Value
                );

            String^ currentStatus =
                Convert::ToString(
                    row->Cells["Status"]->Value
                );

            bool currentlyActive =
                currentStatus->Equals(
                    L"Active",
                    StringComparison::OrdinalIgnoreCase
                );

            String^ newStatus =
                currentlyActive
                ? L"Inactive"
                : L"Active";

            try
            {
                auto con =
                    DbConnection::GetConnection();

                std::unique_ptr<sql::PreparedStatement>
                    stmt(
                        con->prepareStatement(
                            "UPDATE teachers "
                            "SET employment_status = ? "
                            "WHERE teacher_id = ?"
                        )
                    );

                stmt->setString(
                    1,
                    msclr::interop::marshal_as<std::string>(
                        newStatus
                    )
                );

                stmt->setInt64(
                    2,
                    teacherId
                );

                stmt->executeUpdate();

                LoadTeachers();

                MessageBox::Show(
                    currentlyActive
                    ? L"Teacher marked as inactive."
                    : L"Teacher marked as active.",
                    L"Teachers",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Information
                );
            }
            catch (sql::SQLException& ex)
            {
                MessageBox::Show(
                    gcnew String(ex.what()),
                    L"Database Error",
                    MessageBoxButtons::OK,
                    MessageBoxIcon::Error
                );
            }
        }


        // =========================================================
        // BACK
        // =========================================================

        System::Void btnBack_Click(
            System::Object^ sender,
            System::EventArgs^ e)
        {
            this->Close();
        }


        // =========================================================
        // INITIALIZE COMPONENTS
        // =========================================================

        void InitializeComponent(void)
        {
            this->mainLayout =
                gcnew TableLayoutPanel();

            this->headerPanel =
                gcnew Panel();

            this->lblTitle =
                gcnew Label();

            this->lblSubtitle =
                gcnew Label();

            this->actionPanel =
                gcnew Panel();

            this->btnRegisterTeacher =
                gcnew Button();

            this->lblSearch =
                gcnew Label();

            this->txtSearch =
                gcnew TextBox();

            this->btnSearch =
                gcnew Button();

            this->lblTeacherCount =
                gcnew Label();

            this->teachersGrid =
                gcnew DataGridView();

            this->buttonPanel =
                gcnew FlowLayoutPanel();

            this->btnViewProfile =
                gcnew Button();

            this->btnEditTeacher =
                gcnew Button();

            this->btnToggleTeacher =
                gcnew Button();

            this->btnBack =
                gcnew Button();


            this->SuspendLayout();


            // =====================================================
            // FORM
            // =====================================================

            this->Text =
                L"Teachers";

            this->StartPosition =
                FormStartPosition::CenterScreen;

            this->FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::Sizable;

            this->MaximizeBox = true;
            this->MinimizeBox = true;

            this->ClientSize =
                System::Drawing::Size(
                    1120,
                    760
                );

            this->MinimumSize =
                System::Drawing::Size(
                    950,
                    650
                );

            this->BackColor =
                Color::FromArgb(
                    248, 250, 252
                );


            // =====================================================
            // LAYOUT
            // =====================================================

            this->mainLayout->Dock =
                DockStyle::Fill;

            this->mainLayout->Padding =
                System::Windows::Forms::Padding(
                    20
                );

            this->mainLayout->ColumnCount = 1;
            this->mainLayout->RowCount = 5;

            this->mainLayout->ColumnStyles->Add(
                gcnew ColumnStyle(
                    SizeType::Percent,
                    100.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    86.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    70.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    38.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Percent,
                    100.0F
                )
            );

            this->mainLayout->RowStyles->Add(
                gcnew RowStyle(
                    SizeType::Absolute,
                    58.0F
                )
            );


            // =====================================================
            // HEADER
            // =====================================================

            this->headerPanel->Dock =
                DockStyle::Fill;

            this->headerPanel->Margin =
                System::Windows::Forms::Padding(
                    0,
                    0,
                    0,
                    12
                );

            this->headerPanel->BackColor =
                Color::FromArgb(
                    30, 41, 59
                );

            this->headerPanel->Padding =
                System::Windows::Forms::Padding(
                    20,
                    8,
                    20,
                    8
                );


            this->lblTitle->AutoSize = false;
            this->lblTitle->Dock =
                DockStyle::Top;
            this->lblTitle->Height = 44;
            this->lblTitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    16,
                    FontStyle::Bold
                );
            this->lblTitle->ForeColor =
                Color::White;
            this->lblTitle->Text =
                L"Teachers";
            this->lblTitle->TextAlign =
                ContentAlignment::MiddleLeft;


            this->lblSubtitle->AutoSize = false;
            this->lblSubtitle->Dock =
                DockStyle::Fill;
            this->lblSubtitle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F,
                    System::Drawing::FontStyle::Regular
                );

            this->lblSubtitle->ForeColor =
                Color::Gainsboro;
            this->lblSubtitle->Text =
                L"Register, search and manage teacher records.";
            this->lblSubtitle->TextAlign =
                ContentAlignment::TopLeft;


            this->headerPanel->Controls->Add(
                this->lblSubtitle
            );

            this->headerPanel->Controls->Add(
                this->lblTitle
            );


            // =====================================================
            // ACTION PANEL
            // =====================================================

            this->actionPanel->Dock =
                DockStyle::Fill;

            this->actionPanel->Margin =
                System::Windows::Forms::Padding(
                    0,
                    0,
                    0,
                    8
                );

            this->actionPanel->BackColor =
                Color::White;

            this->actionPanel->Padding =
                System::Windows::Forms::Padding(
                    15,
                    12,
                    15,
                    10
                );


            this->btnRegisterTeacher->Text =
                L"+ Register Teacher";

            this->btnRegisterTeacher->Location =
                Point(15, 13);

            this->btnRegisterTeacher->Size =
                System::Drawing::Size(
                    180,
                    42
                );

            this->btnRegisterTeacher->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F,
                    FontStyle::Bold
                );

            this->btnRegisterTeacher->BackColor =
                Color::FromArgb(
                    30, 41, 59
                );

            this->btnRegisterTeacher->ForeColor =
                Color::White;

            this->btnRegisterTeacher->FlatStyle =
                FlatStyle::Flat;

            this->btnRegisterTeacher->FlatAppearance->BorderSize =
                0;

            this->btnRegisterTeacher->Cursor =
                Cursors::Hand;


            this->lblSearch->Text =
                L"Search";

            this->lblSearch->AutoSize = true;

            this->lblSearch->Location =
                Point(460, 24);

            this->lblSearch->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F
                );


            this->txtSearch->Location =
                Point(515, 19);

            this->txtSearch->Size =
                System::Drawing::Size(
                    340,
                    30
                );


            this->btnSearch->Text =
                L"Search";

            this->btnSearch->Location =
                Point(865, 18);

            this->btnSearch->Size =
                System::Drawing::Size(
                    90,
                    32
                );


            this->actionPanel->Controls->Add(
                this->btnRegisterTeacher
            );

            this->actionPanel->Controls->Add(
                this->lblSearch
            );

            this->actionPanel->Controls->Add(
                this->txtSearch
            );

            this->actionPanel->Controls->Add(
                this->btnSearch
            );


            // =====================================================
            // COUNT
            // =====================================================

            this->lblTeacherCount->Dock =
                DockStyle::Fill;

            this->lblTeacherCount->Text =
                L"Teachers: 0";

            this->lblTeacherCount->ForeColor =
                Color::DimGray;

            this->lblTeacherCount->TextAlign =
                ContentAlignment::MiddleLeft;


            // =====================================================
            // GRID
            // =====================================================

            this->teachersGrid->Dock =
                DockStyle::Fill;

            this->teachersGrid->AllowUserToAddRows = false;
            this->teachersGrid->AllowUserToDeleteRows = false;
            this->teachersGrid->AllowUserToResizeRows = false;
            this->teachersGrid->AutoGenerateColumns = false;
            this->teachersGrid->ReadOnly = true;
            this->teachersGrid->MultiSelect = false;
            this->teachersGrid->SelectionMode =
                DataGridViewSelectionMode::FullRowSelect;
            this->teachersGrid->RowHeadersVisible = false;
            this->teachersGrid->BackgroundColor = Color::White;
            this->teachersGrid->BorderStyle =
                BorderStyle::None;

            this->teachersGrid->ColumnHeadersHeight = 42;
            this->teachersGrid->EnableHeadersVisualStyles = false;

            this->teachersGrid->ColumnHeadersDefaultCellStyle->BackColor =
                Color::FromArgb(30, 41, 59);

            this->teachersGrid->ColumnHeadersDefaultCellStyle->ForeColor =
                Color::White;

            this->teachersGrid->ColumnHeadersDefaultCellStyle->Font =
                gcnew System::Drawing::Font(
                    L"Segoe UI",
                    9.5F,
                    FontStyle::Bold
                );

            this->teachersGrid->RowTemplate->Height = 34;


            DataGridViewTextBoxColumn^ teacherIdColumn =
                gcnew DataGridViewTextBoxColumn();

            teacherIdColumn->Name =
                L"TeacherId";

            teacherIdColumn->HeaderText =
                L"Teacher ID";

            teacherIdColumn->Visible = false;

            this->teachersGrid->Columns->Add(
                teacherIdColumn
            );


            DataGridViewTextBoxColumn^ staffColumn =
                gcnew DataGridViewTextBoxColumn();

            staffColumn->Name =
                L"StaffNumber";

            staffColumn->HeaderText =
                L"Staff Number";

            staffColumn->Width = 155;

            this->teachersGrid->Columns->Add(
                staffColumn
            );


            DataGridViewTextBoxColumn^ nameColumn =
                gcnew DataGridViewTextBoxColumn();

            nameColumn->Name =
                L"TeacherName";

            nameColumn->HeaderText =
                L"Teacher Name";

            nameColumn->AutoSizeMode =
                DataGridViewAutoSizeColumnMode::Fill;

            this->teachersGrid->Columns->Add(
                nameColumn
            );


            DataGridViewTextBoxColumn^ genderColumn =
                gcnew DataGridViewTextBoxColumn();

            genderColumn->Name =
                L"Gender";

            genderColumn->HeaderText =
                L"Gender";

            genderColumn->Width = 100;

            this->teachersGrid->Columns->Add(
                genderColumn
            );


            DataGridViewTextBoxColumn^ phoneColumn =
                gcnew DataGridViewTextBoxColumn();

            phoneColumn->Name =
                L"Phone";

            phoneColumn->HeaderText =
                L"Phone";

            phoneColumn->Width = 140;

            this->teachersGrid->Columns->Add(
                phoneColumn
            );


            DataGridViewTextBoxColumn^ statusColumn =
                gcnew DataGridViewTextBoxColumn();

            statusColumn->Name =
                L"Status";

            statusColumn->HeaderText =
                L"Status";

            statusColumn->Width = 100;

            this->teachersGrid->Columns->Add(
                statusColumn
            );


            // =====================================================
            // BUTTONS
            // =====================================================

            this->buttonPanel->Dock =
                DockStyle::Fill;

            this->buttonPanel->FlowDirection =
                FlowDirection::RightToLeft;

            this->buttonPanel->WrapContents = false;

            this->buttonPanel->Padding =
                System::Windows::Forms::Padding(
                    0,
                    8,
                    0,
                    0
                );


            this->btnBack->Text =
                L"Back to Dashboard";

            this->btnBack->Size =
                System::Drawing::Size(
                    150,
                    40
                );


            this->btnToggleTeacher->Text =
                L"Activate / Deactivate";

            this->btnToggleTeacher->Size =
                System::Drawing::Size(
                    155,
                    40
                );

            this->btnToggleTeacher->Enabled = false;


            this->btnEditTeacher->Text =
                L"Edit Teacher";

            this->btnEditTeacher->Size =
                System::Drawing::Size(
                    120,
                    40
                );

            this->btnEditTeacher->Enabled = false;


            this->btnViewProfile->Text =
                L"View Profile";

            this->btnViewProfile->Size =
                System::Drawing::Size(
                    120,
                    40
                );

            this->btnViewProfile->Enabled = false;


            this->buttonPanel->Controls->Add(
                this->btnBack
            );

            this->buttonPanel->Controls->Add(
                this->btnToggleTeacher
            );

            this->buttonPanel->Controls->Add(
                this->btnEditTeacher
            );

            this->buttonPanel->Controls->Add(
                this->btnViewProfile
            );


            // =====================================================
            // MAIN CONTROLS
            // =====================================================

            this->mainLayout->Controls->Add(
                this->headerPanel,
                0,
                0
            );

            this->mainLayout->Controls->Add(
                this->actionPanel,
                0,
                1
            );

            this->mainLayout->Controls->Add(
                this->lblTeacherCount,
                0,
                2
            );

            this->mainLayout->Controls->Add(
                this->teachersGrid,
                0,
                3
            );

            this->mainLayout->Controls->Add(
                this->buttonPanel,
                0,
                4
            );

            this->Controls->Add(
                this->mainLayout
            );


            // =====================================================
            // EVENTS
            // =====================================================

            this->btnRegisterTeacher->Click +=
                gcnew EventHandler(
                    this,
                    &TeacherManagement::btnRegisterTeacher_Click
                );

            this->btnSearch->Click +=
                gcnew EventHandler(
                    this,
                    &TeacherManagement::btnSearch_Click
                );

            this->txtSearch->KeyDown +=
                gcnew KeyEventHandler(
                    this,
                    &TeacherManagement::txtSearch_KeyDown
                );

            this->teachersGrid->SelectionChanged +=
                gcnew EventHandler(
                    this,
                    &TeacherManagement::teachersGrid_SelectionChanged
                );

            this->btnViewProfile->Click +=
                gcnew EventHandler(
                    this,
                    &TeacherManagement::btnViewProfile_Click
                );

            this->btnEditTeacher->Click +=
                gcnew EventHandler(
                    this,
                    &TeacherManagement::btnEditTeacher_Click
                );

            this->btnToggleTeacher->Click +=
                gcnew EventHandler(
                    this,
                    &TeacherManagement::btnToggleTeacher_Click
                );

            this->btnBack->Click +=
                gcnew EventHandler(
                    this,
                    &TeacherManagement::btnBack_Click
                );

            this->ResumeLayout(false);
        }


    public:

        TeacherManagement()
        {
            InitializeComponent();
            LoadTeachers();
        }
    };
}
