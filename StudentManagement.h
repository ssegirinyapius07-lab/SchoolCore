#pragma once
#include "StudentRegistration.h"
#include "DbConnection.h"

#include <msclr/marshal_cppstd.h>

namespace SchoolCore
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class StudentManagement : public System::Windows::Forms::Form
	{
	public:
		StudentManagement(void)
		{
			InitializeComponent();

			this->btnSearch->Click +=
				gcnew System::EventHandler(
					this,
					&StudentManagement::btnSearch_Click
				);

			this->txtSearch->KeyDown +=
				gcnew System::Windows::Forms::KeyEventHandler(
					this,
					&StudentManagement::txtSearch_KeyDown
				);

			this->studentsGrid->SelectionChanged +=
				gcnew System::EventHandler(
					this,
					&StudentManagement::studentsGrid_SelectionChanged
				);

			this->btnViewProfile->Click +=
				gcnew System::EventHandler(
					this,
					&StudentManagement::btnViewProfile_Click
				);

			this->btnEditStudent->Click +=
				gcnew System::EventHandler(
					this,
					&StudentManagement::btnEditStudent_Click
				);

			this->btnViewProfile->Enabled = false;
			this->btnEditStudent->Enabled = false;

			LoadStudents();
		}

	protected:
		~StudentManagement()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		System::ComponentModel::Container^ components;

		// Main layout
		System::Windows::Forms::TableLayoutPanel^ mainLayout;

		// Header
		System::Windows::Forms::Panel^ headerPanel;
		System::Windows::Forms::Label^ lblTitle;
		System::Windows::Forms::Label^ lblSubtitle;

		// Actions
		System::Windows::Forms::Panel^ actionPanel;
		System::Windows::Forms::Button^ btnRegisterStudent;

		System::Windows::Forms::Label^ lblSearch;
		System::Windows::Forms::TextBox^ txtSearch;
		System::Windows::Forms::Button^ btnSearch;

		// Student list
		System::Windows::Forms::Label^ lblStudentCount;
		System::Windows::Forms::DataGridView^ studentsGrid;

		// Bottom action
		System::Windows::Forms::FlowLayoutPanel^ buttonPanel;
		System::Windows::Forms::Button^ btnViewProfile;
		System::Windows::Forms::Button^ btnEditStudent;
		System::Windows::Forms::Button^ btnBack;

		System::Void btnRegisterStudent_Click(
			System::Object^ sender,
			System::EventArgs^ e)
		{
			StudentRegistration^ form =
				gcnew StudentRegistration();

			if (form->ShowDialog(this) ==
				System::Windows::Forms::DialogResult::OK)
			{
				LoadStudents();
			}
		}

		System::Void btnBack_Click(
			System::Object^ sender,
			System::EventArgs^ e)
		{
			this->Close();
		}

#pragma region Windows Form Designer generated code

		void LoadStudents()
		{
			try
			{
				auto con = DbConnection::GetConnection();

				String^ searchText =
					this->txtSearch->Text->Trim();

				String^ searchPattern =
					L"%" + searchText + L"%";

				std::unique_ptr<sql::PreparedStatement> stmt;

				const char* baseSql =
					"SELECT "
					"s.student_id, "
					"s.registration_number, "
					"s.first_name, "
					"s.middle_name, "
					"s.last_name, "
					"s.status, "
					"c.class_name, "
					"st.stream_name "
					"FROM students s "
					"LEFT JOIN enrollments e "
					"ON e.enrollment_id = ("
						"SELECT e2.enrollment_id "
						"FROM enrollments e2 "
						"WHERE e2.student_id = s.student_id "
						"AND e2.status = 'Active' "
						"ORDER BY e2.enrollment_date DESC, e2.enrollment_id DESC "
						"LIMIT 1"
					") "
					"LEFT JOIN classes c "
					"ON c.class_id = e.class_id "
					"LEFT JOIN streams st "
					"ON st.stream_id = e.stream_id ";

				if (String::IsNullOrWhiteSpace(searchText))
				{
					stmt.reset(
						con->prepareStatement(
							(std::string(baseSql) +
							"ORDER BY s.student_id DESC").c_str()
						)
					);
				}
				else
				{
					stmt.reset(
						con->prepareStatement(
							(std::string(baseSql) +
							"WHERE s.registration_number LIKE ? "
							"OR s.first_name LIKE ? "
							"OR s.middle_name LIKE ? "
							"OR s.last_name LIKE ? "
							"OR CONCAT_WS(' ', s.first_name, s.middle_name, s.last_name) LIKE ? "
							"ORDER BY s.student_id DESC").c_str()
						)
					);

					std::string pattern =
						msclr::interop::marshal_as<std::string>(
							searchPattern
						);

					for (int i = 1; i <= 5; i++)
					{
						stmt->setString(i, pattern);
					}
				}

				std::unique_ptr<sql::ResultSet> result(
					stmt->executeQuery()
				);

				this->studentsGrid->Rows->Clear();

				int studentCount = 0;

				while (result->next())
				{
					String^ registration =
						gcnew String(
							result->getString("registration_number").c_str()
						);

					String^ firstName =
						gcnew String(
							result->getString("first_name").c_str()
						);

					String^ lastName =
						gcnew String(
							result->getString("last_name").c_str()
						);

					String^ middleName = L"";

					if (!result->isNull("middle_name"))
					{
						middleName =
							gcnew String(
								result->getString("middle_name").c_str()
							);
					}

					String^ className = L"-";
					String^ streamName = L"-";

					if (!result->isNull("class_name"))
					{
						className =
							gcnew String(
								result->getString("class_name").c_str()
							);
					}

					if (!result->isNull("stream_name"))
					{
						streamName =
							gcnew String(
								result->getString("stream_name").c_str()
							);
					}

					String^ status =
						gcnew String(
							result->getString("status").c_str()
						);

					String^ fullName =
						firstName + L" ";

					if (!String::IsNullOrWhiteSpace(middleName))
					{
						fullName += middleName + L" ";
					}

					fullName += lastName;

					this->studentsGrid->Rows->Add(
						result->getInt64("student_id"),
						registration,
						fullName,
						className,
						streamName,
						status
					);

					studentCount++;
				}

				this->lblStudentCount->Text =
					(String::IsNullOrWhiteSpace(searchText)
						? L"Students: "
						: L"Matching Students: ") +
					studentCount.ToString();

				bool hasSelection =
					this->studentsGrid->SelectedRows->Count > 0;

				this->btnViewProfile->Enabled =
					hasSelection;

				this->btnEditStudent->Enabled =
					hasSelection;
			}
			catch (sql::SQLException& ex)
			{
				MessageBox::Show(
					gcnew String(ex.what()),
					"Database Error",
					MessageBoxButtons::OK,
					MessageBoxIcon::Error
				);
			}
		}

		System::Void btnSearch_Click(
			System::Object^ sender,
			System::EventArgs^ e)
		{
			LoadStudents();
		}

		System::Void txtSearch_KeyDown(
			System::Object^ sender,
			System::Windows::Forms::KeyEventArgs^ e)
		{
			if (e->KeyCode == Keys::Enter)
			{
				LoadStudents();
				e->SuppressKeyPress = true;
			}
		}

		System::Void studentsGrid_SelectionChanged(
			System::Object^ sender,
			System::EventArgs^ e)
		{
			this->btnViewProfile->Enabled =
				this->studentsGrid->SelectedRows->Count > 0;

			this->btnEditStudent->Enabled =
				this->studentsGrid->SelectedRows->Count > 0;
		}

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
				L"Segoe UI", 9.5F, FontStyle::Bold);
			label->ForeColor = Color::FromArgb(71, 85, 105);
			label->TextAlign = ContentAlignment::MiddleLeft;
			label->Margin = System::Windows::Forms::Padding(0, 3, 12, 3);

			Label^ value = gcnew Label();
			value->Text =
				String::IsNullOrWhiteSpace(valueText)
					? L"Not provided"
					: valueText;
			value->Dock = DockStyle::Fill;
			value->Font = gcnew System::Drawing::Font(
				L"Segoe UI", 9.5F, FontStyle::Regular);
			value->ForeColor = Color::FromArgb(30, 41, 59);
			value->Margin = System::Windows::Forms::Padding(0, 3, 0, 3);
			value->TextAlign = ContentAlignment::MiddleLeft;

			layout->Controls->Add(label, 0, row);
			layout->Controls->Add(value, 1, row);
		}


		System::Void btnViewProfile_Click(
			System::Object^ sender,
			System::EventArgs^ e)
		{
			if (this->studentsGrid->SelectedRows->Count == 0)
			{
				MessageBox::Show(
					L"Please select a student.",
					L"Students",
					MessageBoxButtons::OK,
					MessageBoxIcon::Warning
				);
				return;
			}

			String^ registration =
				Convert::ToString(
					this->studentsGrid
					->SelectedRows[0]
					->Cells["RegistrationNumber"]
					->Value
				);

			try
			{
				auto con = DbConnection::GetConnection();

				std::unique_ptr<sql::PreparedStatement> stmt(
					con->prepareStatement(
						"SELECT "
						"s.registration_number, "
						"s.first_name, "
						"s.middle_name, "
						"s.last_name, "
						"s.date_of_birth, "
						"s.gender, "
						"s.admission_date, "
						"s.home_address, "
						"s.status, "
						"c.class_name, "
						"st.stream_name, "
						"e.enrollment_date, "
						"g.full_name AS guardian_name, "
						"g.relationship AS guardian_relationship, "
						"g.phone_number AS guardian_phone, "
						"g.alternative_phone AS guardian_alternative_phone, "
						"g.email AS guardian_email, "
						"g.address AS guardian_address "
						"FROM students s "
						"LEFT JOIN enrollments e "
						"ON e.enrollment_id = ("
							"SELECT e2.enrollment_id "
							"FROM enrollments e2 "
							"WHERE e2.student_id = s.student_id "
							"AND e2.status = 'Active' "
							"ORDER BY e2.enrollment_date DESC, e2.enrollment_id DESC "
							"LIMIT 1"
						") "
						"LEFT JOIN classes c ON c.class_id = e.class_id "
						"LEFT JOIN streams st ON st.stream_id = e.stream_id "
						"LEFT JOIN student_guardians sg "
						"ON sg.student_id = s.student_id "
						"AND sg.is_primary = 1 "
						"LEFT JOIN guardians g ON g.guardian_id = sg.guardian_id "
						"WHERE s.registration_number = ? "
						"LIMIT 1"
					)
				);

				stmt->setString(
					1,
					msclr::interop::marshal_as<std::string>(
						registration
					)
				);

				std::unique_ptr<sql::ResultSet> result(
					stmt->executeQuery()
				);

				if (!result->next())
				{
					MessageBox::Show(
						L"Student record could not be found.",
						L"Students",
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
							result->getString("middle_name").c_str()
						);
				}

				String^ fullName =
					gcnew String(result->getString("first_name").c_str()) +
					L" " +
					middleName +
					(middleName->Length > 0 ? L" " : L"") +
					gcnew String(result->getString("last_name").c_str());

				String^ className =
					result->isNull("class_name")
						? L"Not assigned"
						: gcnew String(result->getString("class_name").c_str());

				String^ streamName =
					result->isNull("stream_name")
						? L"Not assigned"
						: gcnew String(result->getString("stream_name").c_str());

				String^ guardianName =
					result->isNull("guardian_name")
						? L"Not recorded"
						: gcnew String(result->getString("guardian_name").c_str());

				String^ guardianRelationship =
					result->isNull("guardian_relationship")
						? L""
						: gcnew String(result->getString("guardian_relationship").c_str());

				String^ guardianPhone =
					result->isNull("guardian_phone")
						? L""
						: gcnew String(result->getString("guardian_phone").c_str());

				String^ guardianEmail =
					result->isNull("guardian_email")
						? L""
						: gcnew String(
							result->getString("guardian_email").c_str()
						);

				String^ guardianAlternativePhone =
					result->isNull("guardian_alternative_phone")
						? L""
						: gcnew String(
							result->getString("guardian_alternative_phone").c_str()
						);

				String^ guardianAddress =
					result->isNull("guardian_address")
						? L""
						: gcnew String(
							result->getString("guardian_address").c_str()
						);

				String^ gender =
					gcnew String(result->getString("gender").c_str());

				String^ dateOfBirth =
					gcnew String(result->getString("date_of_birth").c_str());

				String^ admissionDate =
					gcnew String(result->getString("admission_date").c_str());

				String^ enrollmentDate =
					result->isNull("enrollment_date")
						? L"Not recorded"
						: gcnew String(
							result->getString("enrollment_date").c_str()
						);

				String^ enrollmentStatus =
					result->isNull("enrollment_date")
						? L"Not assigned"
						: L"Active";

				String^ status =
					gcnew String(result->getString("status").c_str());

				String^ homeAddress =
					gcnew String(result->getString("home_address").c_str());


				// =====================================================
				// PROFESSIONAL PROFILE DIALOG
				// =====================================================

				Form^ profileForm = gcnew Form();

				profileForm->Text = L"Student Profile";
				profileForm->StartPosition = FormStartPosition::CenterParent;
				profileForm->FormBorderStyle =
					System::Windows::Forms::FormBorderStyle::FixedSingle;
				profileForm->MaximizeBox = false;
				profileForm->MinimizeBox = false;
				profileForm->ShowInTaskbar = false;
				profileForm->ClientSize = System::Drawing::Size(720, 680);
				profileForm->BackColor = Color::FromArgb(248, 250, 252);

				// Header
				Panel^ profileHeader = gcnew Panel();
				profileHeader->Dock = DockStyle::Top;
				profileHeader->Height = 125;
				profileHeader->BackColor = Color::FromArgb(30, 41, 59);
				profileHeader->Padding = System::Windows::Forms::Padding(24, 16, 24, 12);

				Label^ nameLabel = gcnew Label();
				nameLabel->Text = fullName;
				nameLabel->Dock = DockStyle::Top;
				nameLabel->Height = 42;
				nameLabel->Font = gcnew System::Drawing::Font(
					L"Segoe UI Semibold", 20.0F, FontStyle::Bold);
				nameLabel->ForeColor = Color::White;
				nameLabel->TextAlign = ContentAlignment::MiddleLeft;

				Label^ registrationLabel = gcnew Label();
				registrationLabel->Text = L"Registration No.  " + registration;
				registrationLabel->Dock = DockStyle::Top;
				registrationLabel->Height = 28;
				registrationLabel->Font = gcnew System::Drawing::Font(
					L"Segoe UI", 10.0F);
				registrationLabel->ForeColor = Color::Gainsboro;
				registrationLabel->TextAlign = ContentAlignment::MiddleLeft;

				Label^ statusLabel = gcnew Label();
				statusLabel->Text = L"Status: " + status;
				statusLabel->Dock = DockStyle::Top;
				statusLabel->Height = 28;
				statusLabel->Font = gcnew System::Drawing::Font(
					L"Segoe UI Semibold", 9.0F, FontStyle::Bold);
				statusLabel->ForeColor =
					status->Equals(
						L"Active",
						StringComparison::OrdinalIgnoreCase)
						? Color::FromArgb(167, 243, 208)
						: Color::FromArgb(254, 202, 202);
				statusLabel->TextAlign = ContentAlignment::MiddleLeft;

				profileHeader->Controls->Add(statusLabel);
				profileHeader->Controls->Add(registrationLabel);
				profileHeader->Controls->Add(nameLabel);


				// Scrollable content
				Panel^ contentPanel = gcnew Panel();
				contentPanel->Dock = DockStyle::Fill;
				contentPanel->AutoScroll = true;
				contentPanel->Padding = System::Windows::Forms::Padding(20);

				TableLayoutPanel^ contentLayout = gcnew TableLayoutPanel();
				contentLayout->Dock = DockStyle::Top;
				contentLayout->AutoSize = true;
				contentLayout->ColumnCount = 1;
				contentLayout->RowCount = 3;
				contentLayout->ColumnStyles->Add(
					gcnew ColumnStyle(SizeType::Percent, 100.0F));


				// Student section
				GroupBox^ studentSection = gcnew GroupBox();
				studentSection->Text = L"Student Information";
				studentSection->Dock = DockStyle::Top;
				studentSection->Height = 285;
				studentSection->Padding =
					System::Windows::Forms::Padding(14, 18, 14, 10);
				studentSection->Margin =
					System::Windows::Forms::Padding(0, 0, 0, 16);

				TableLayoutPanel^ studentLayout = gcnew TableLayoutPanel();
				studentLayout->Dock = DockStyle::Fill;
				studentLayout->ColumnCount = 2;
				studentLayout->RowCount = 6;
				studentLayout->ColumnStyles->Add(
					gcnew ColumnStyle(SizeType::Absolute, 150.0F));
				studentLayout->ColumnStyles->Add(
					gcnew ColumnStyle(SizeType::Percent, 100.0F));

				for (int i = 0; i < 6; i++)
				{
					studentLayout->RowStyles->Add(
						gcnew RowStyle(SizeType::Absolute, 36.0F));
				}

				AddProfileField(studentLayout, 0, L"Full Name", fullName);
				AddProfileField(studentLayout, 1, L"Gender", gender);
				AddProfileField(studentLayout, 2, L"Date of Birth", dateOfBirth);
				AddProfileField(studentLayout, 3, L"Admission Date", admissionDate);
				AddProfileField(studentLayout, 4, L"Home Address", homeAddress);
				AddProfileField(studentLayout, 5, L"Student Status", status);

				studentSection->Controls->Add(studentLayout);


				// Enrollment section
				GroupBox^ enrollmentSection = gcnew GroupBox();
				enrollmentSection->Text = L"Enrollment Information";
				enrollmentSection->Dock = DockStyle::Top;
				enrollmentSection->Height = 170;
				enrollmentSection->Padding =
					System::Windows::Forms::Padding(14, 18, 14, 10);
				enrollmentSection->Margin =
					System::Windows::Forms::Padding(0, 0, 0, 12);

				TableLayoutPanel^ enrollmentLayout = gcnew TableLayoutPanel();
				enrollmentLayout->Dock = DockStyle::Fill;
				enrollmentLayout->ColumnCount = 2;
				enrollmentLayout->RowCount = 4;
				enrollmentLayout->ColumnStyles->Add(
					gcnew ColumnStyle(SizeType::Absolute, 150.0F));
				enrollmentLayout->ColumnStyles->Add(
					gcnew ColumnStyle(SizeType::Percent, 100.0F));

				for (int i = 0; i < 4; i++)
				{
					enrollmentLayout->RowStyles->Add(
						gcnew RowStyle(SizeType::Absolute, 30.0F));
				}

				AddProfileField(enrollmentLayout, 0, L"Class", className);
				AddProfileField(enrollmentLayout, 1, L"Stream", streamName);
				AddProfileField(enrollmentLayout, 2, L"Enrollment Date", enrollmentDate);
				AddProfileField(enrollmentLayout, 3, L"Enrollment Status", enrollmentStatus);

				enrollmentSection->Controls->Add(enrollmentLayout);


				// Guardian section
				GroupBox^ guardianSection = gcnew GroupBox();
				guardianSection->Text = L"Guardian Information";
				guardianSection->Dock = DockStyle::Top;
				guardianSection->Height = 235;
				guardianSection->Padding =
					System::Windows::Forms::Padding(14, 18, 14, 10);

				TableLayoutPanel^ guardianLayout = gcnew TableLayoutPanel();
				guardianLayout->Dock = DockStyle::Fill;
				guardianLayout->ColumnCount = 2;
				guardianLayout->RowCount = 6;
				guardianLayout->ColumnStyles->Add(
					gcnew ColumnStyle(SizeType::Absolute, 150.0F));
				guardianLayout->ColumnStyles->Add(
					gcnew ColumnStyle(SizeType::Percent, 100.0F));

				for (int i = 0; i < 6; i++)
				{
					guardianLayout->RowStyles->Add(
						gcnew RowStyle(SizeType::Absolute, 30.0F));
				}

				AddProfileField(guardianLayout, 0, L"Full Name", guardianName);
				AddProfileField(guardianLayout, 1, L"Relationship", guardianRelationship);
				AddProfileField(guardianLayout, 2, L"Phone", guardianPhone);
				AddProfileField(guardianLayout, 3, L"Alternative Phone", guardianAlternativePhone);
				AddProfileField(guardianLayout, 4, L"Email", guardianEmail);
				AddProfileField(guardianLayout, 5, L"Address", guardianAddress);

				guardianSection->Controls->Add(guardianLayout);


				contentLayout->Controls->Add(studentSection, 0, 0);
				contentLayout->Controls->Add(enrollmentSection, 0, 1);
				contentLayout->Controls->Add(guardianSection, 0, 2);
				contentPanel->Controls->Add(contentLayout);


				// Footer
				Panel^ footerPanel = gcnew Panel();
				footerPanel->Dock = DockStyle::Bottom;
				footerPanel->Height = 60;
				footerPanel->Padding =
					System::Windows::Forms::Padding(20, 8, 20, 10);

				Button^ closeButton = gcnew Button();
				closeButton->Text = L"Close";
				closeButton->Dock = DockStyle::Right;
				closeButton->Width = 110;
				closeButton->Height = 38;
				closeButton->BackColor = Color::FromArgb(30, 41, 59);
				closeButton->ForeColor = Color::White;
				closeButton->FlatStyle = FlatStyle::Flat;
				closeButton->FlatAppearance->BorderSize = 0;
				closeButton->DialogResult =
					System::Windows::Forms::DialogResult::Cancel;

				footerPanel->Controls->Add(closeButton);

				profileForm->Controls->Add(contentPanel);
				profileForm->Controls->Add(footerPanel);
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


		System::Void btnEditStudent_Click(
			System::Object^ sender,
			System::EventArgs^ e)
		{
			if (this->studentsGrid->SelectedRows->Count == 0)
			{
				MessageBox::Show(
					L"Please select a student.",
					L"Students",
					MessageBoxButtons::OK,
					MessageBoxIcon::Warning
				);
				return;
			}

			long long studentId =
				Convert::ToInt64(
					this->studentsGrid
					->SelectedRows[0]
					->Cells["StudentId"]
					->Value
				);

			StudentRegistration^ form =
				gcnew StudentRegistration(studentId);

			if (
				form->ShowDialog(this) ==
				System::Windows::Forms::DialogResult::OK
			)
			{
				LoadStudents();
			}
		}


		void InitializeComponent(void)
		{
			this->components =
				gcnew System::ComponentModel::Container();

			// ============================================================
			// CONTROLS
			// ============================================================

			this->mainLayout =
				gcnew System::Windows::Forms::TableLayoutPanel();

			this->headerPanel =
				gcnew System::Windows::Forms::Panel();

			this->lblTitle =
				gcnew System::Windows::Forms::Label();

			this->lblSubtitle =
				gcnew System::Windows::Forms::Label();

			this->actionPanel =
				gcnew System::Windows::Forms::Panel();

			this->btnRegisterStudent =
				gcnew System::Windows::Forms::Button();
			this->btnRegisterStudent->Click +=
				gcnew System::EventHandler(
					this,
					&StudentManagement::btnRegisterStudent_Click
				);

			this->lblSearch =
				gcnew System::Windows::Forms::Label();

			this->txtSearch =
				gcnew System::Windows::Forms::TextBox();

			this->btnSearch =
				gcnew System::Windows::Forms::Button();

			this->lblStudentCount =
				gcnew System::Windows::Forms::Label();

			this->studentsGrid =
				gcnew System::Windows::Forms::DataGridView();

			this->buttonPanel =
				gcnew System::Windows::Forms::FlowLayoutPanel();

			this->btnViewProfile =
				gcnew System::Windows::Forms::Button();

			this->btnEditStudent =
				gcnew System::Windows::Forms::Button();

			this->btnBack =
				gcnew System::Windows::Forms::Button();


			// ============================================================
			// FONTS
			// ============================================================

			System::Drawing::Font^ regularFont =
				gcnew System::Drawing::Font(
					L"Segoe UI",
					9.5F,
					System::Drawing::FontStyle::Regular
				);

			System::Drawing::Font^ boldFont =
				gcnew System::Drawing::Font(
					L"Segoe UI",
					9.5F,
					System::Drawing::FontStyle::Bold
				);


			this->SuspendLayout();


			// ============================================================
			// FORM
			// ============================================================

			this->AutoScaleDimensions =
				System::Drawing::SizeF(96, 96);

			this->AutoScaleMode =
				System::Windows::Forms::AutoScaleMode::Dpi;

			this->BackColor =
				System::Drawing::Color::FromArgb(
					248, 250, 252
				);

			this->ClientSize =
				System::Drawing::Size(1100, 760);

			this->MinimumSize =
				System::Drawing::Size(920, 650);

			this->FormBorderStyle =
				System::Windows::Forms::FormBorderStyle::Sizable;

			this->MaximizeBox = true;
			this->MinimizeBox = true;

			this->StartPosition =
				System::Windows::Forms::FormStartPosition::CenterScreen;

			this->Name =
				L"StudentManagement";

			this->Text =
				L"Students";


			// ============================================================
			// MAIN LAYOUT
			// ============================================================

			this->mainLayout->Dock =
				System::Windows::Forms::DockStyle::Fill;

			this->mainLayout->Padding =
				System::Windows::Forms::Padding(20);

			this->mainLayout->ColumnCount = 1;
			this->mainLayout->RowCount = 5;

			this->mainLayout->ColumnStyles->Add(
				gcnew System::Windows::Forms::ColumnStyle(
					System::Windows::Forms::SizeType::Percent,
					100.0F
				)
			);

			this->mainLayout->RowStyles->Add(
				gcnew System::Windows::Forms::RowStyle(
					System::Windows::Forms::SizeType::Absolute,
					86
				)
			);

			this->mainLayout->RowStyles->Add(
				gcnew System::Windows::Forms::RowStyle(
					System::Windows::Forms::SizeType::Absolute,
					70
				)
			);

			this->mainLayout->RowStyles->Add(
				gcnew System::Windows::Forms::RowStyle(
					System::Windows::Forms::SizeType::Absolute,
					38
				)
			);

			this->mainLayout->RowStyles->Add(
				gcnew System::Windows::Forms::RowStyle(
					System::Windows::Forms::SizeType::Percent,
					100.0F
				)
			);

			this->mainLayout->RowStyles->Add(
				gcnew System::Windows::Forms::RowStyle(
					System::Windows::Forms::SizeType::Absolute,
					58
				)
			);


			// ============================================================
			// HEADER
			// ============================================================

			this->headerPanel->Dock =
				System::Windows::Forms::DockStyle::Fill;

			this->headerPanel->Margin =
				System::Windows::Forms::Padding(
					0, 0, 0, 12
				);

			this->headerPanel->BackColor =
				System::Drawing::Color::FromArgb(
					30, 41, 59
				);

			this->headerPanel->Padding =
				System::Windows::Forms::Padding(
					20, 8, 20, 8
				);


			// Title
			this->lblTitle->AutoSize = false;

			this->lblTitle->Dock =
				System::Windows::Forms::DockStyle::Top;

			this->lblTitle->Height = 44;

			this->lblTitle->Font =
				gcnew System::Drawing::Font(
					L"Segoe UI",
					16,
					System::Drawing::FontStyle::Bold
				);

			this->lblTitle->ForeColor =
				System::Drawing::Color::White;

			this->lblTitle->Text =
				L"Students";

			this->lblTitle->TextAlign =
				System::Drawing::ContentAlignment::MiddleLeft;

			this->lblTitle->UseCompatibleTextRendering = true;


			// Subtitle
			this->lblSubtitle->AutoSize = false;

			this->lblSubtitle->Dock =
				System::Windows::Forms::DockStyle::Fill;

			this->lblSubtitle->Font =
				gcnew System::Drawing::Font(
					L"Segoe UI",
					9.5F,
					System::Drawing::FontStyle::Regular
				);

			this->lblSubtitle->ForeColor =
				System::Drawing::Color::Gainsboro;

			this->lblSubtitle->Text =
				L"Register, search and manage student records.";

			this->lblSubtitle->TextAlign =
				System::Drawing::ContentAlignment::TopLeft;

			this->lblSubtitle->Padding =
				System::Windows::Forms::Padding(
					3, 0, 0, 0
				);

			this->lblSubtitle->UseCompatibleTextRendering = true;


			this->headerPanel->Controls->Add(
				this->lblSubtitle
			);

			this->headerPanel->Controls->Add(
				this->lblTitle
			);


			// ============================================================
			// ACTION PANEL
			// ============================================================

			this->actionPanel->Dock =
				System::Windows::Forms::DockStyle::Fill;

			this->actionPanel->Margin =
				System::Windows::Forms::Padding(
					0, 0, 0, 8
				);

			this->actionPanel->BackColor =
				System::Drawing::Color::White;

			this->actionPanel->Padding =
				System::Windows::Forms::Padding(
					15, 12, 15, 10
				);


			// Register Student
			this->btnRegisterStudent->Text =
				L"+ Register Student";

			this->btnRegisterStudent->Font =
				boldFont;

			this->btnRegisterStudent->BackColor =
				System::Drawing::Color::FromArgb(
					30, 41, 59
				);

			this->btnRegisterStudent->ForeColor =
				System::Drawing::Color::White;

			this->btnRegisterStudent->FlatStyle =
				System::Windows::Forms::FlatStyle::Flat;

			this->btnRegisterStudent->FlatAppearance->BorderSize = 0;

			this->btnRegisterStudent->Cursor =
				System::Windows::Forms::Cursors::Hand;

			this->btnRegisterStudent->Location =
				System::Drawing::Point(15, 13);

			this->btnRegisterStudent->Size =
				System::Drawing::Size(170, 42);


			// Search label
			this->lblSearch->AutoSize = true;

			this->lblSearch->Font =
				regularFont;

			this->lblSearch->Text =
				L"Search";

			this->lblSearch->Location =
				System::Drawing::Point(440, 24);


			// Search box
			this->txtSearch->Font =
				regularFont;

			this->txtSearch->Location =
				System::Drawing::Point(495, 19);

			this->txtSearch->Size =
				System::Drawing::Size(360, 30);


			// Search button
			this->btnSearch->Text =
				L"Search";

			this->btnSearch->Font =
				regularFont;

			this->btnSearch->Location =
				System::Drawing::Point(865, 18);

			this->btnSearch->Size =
				System::Drawing::Size(90, 32);


			this->actionPanel->Controls->Add(
				this->btnRegisterStudent
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


			// ============================================================
			// STUDENT COUNT
			// ============================================================

			this->lblStudentCount->AutoSize = true;

			this->lblStudentCount->Font =
				regularFont;

			this->lblStudentCount->ForeColor =
				System::Drawing::Color::DimGray;

			this->lblStudentCount->Text =
				L"Students: 0";

			this->lblStudentCount->Margin =
				System::Windows::Forms::Padding(3);

			this->lblStudentCount->Dock =
				System::Windows::Forms::DockStyle::Fill;

			this->lblStudentCount->TextAlign =
				System::Drawing::ContentAlignment::MiddleLeft;


			// ============================================================
			// STUDENTS GRID
			// ============================================================

			this->studentsGrid->Dock =
				System::Windows::Forms::DockStyle::Fill;

			this->studentsGrid->Margin =
				System::Windows::Forms::Padding(
					0, 0, 0, 8
				);

			this->studentsGrid->AllowUserToAddRows =
				false;

			this->studentsGrid->AllowUserToDeleteRows =
				false;

			this->studentsGrid->AllowUserToResizeRows =
				false;

			this->studentsGrid->AutoGenerateColumns =
				false;

			this->studentsGrid->BackgroundColor =
				System::Drawing::Color::White;

			this->studentsGrid->BorderStyle =
				System::Windows::Forms::BorderStyle::None;

			this->studentsGrid->CellBorderStyle =
				System::Windows::Forms::DataGridViewCellBorderStyle::SingleHorizontal;

			this->studentsGrid->ColumnHeadersHeight =
				42;

			this->studentsGrid->EnableHeadersVisualStyles =
				false;

			this->studentsGrid->Font =
				regularFont;

			this->studentsGrid->ReadOnly = true;

			this->studentsGrid->RowHeadersVisible = false;

			this->studentsGrid->SelectionMode =
				System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;

			this->studentsGrid->MultiSelect = false;

			this->studentsGrid->AutoSizeRowsMode =
				System::Windows::Forms::DataGridViewAutoSizeRowsMode::None;

			this->studentsGrid->RowTemplate->Height = 34;


			// Header styling
			this->studentsGrid->ColumnHeadersDefaultCellStyle->BackColor =
				System::Drawing::Color::FromArgb(
					30, 41, 59
				);

			this->studentsGrid->ColumnHeadersDefaultCellStyle->ForeColor =
				System::Drawing::Color::White;

			this->studentsGrid->ColumnHeadersDefaultCellStyle->Font =
				boldFont;

			this->studentsGrid->ColumnHeadersDefaultCellStyle->Alignment =
				System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;


			// ============================================================
			// GRID COLUMNS
			// ============================================================

			System::Windows::Forms::DataGridViewTextBoxColumn^ studentIdColumn =
				gcnew System::Windows::Forms::DataGridViewTextBoxColumn();

			studentIdColumn->HeaderText =
				L"Student ID";

			studentIdColumn->Name =
				L"StudentId";

			studentIdColumn->Visible = false;

			this->studentsGrid->Columns->Add(
				studentIdColumn
			);


			System::Windows::Forms::DataGridViewTextBoxColumn^ registrationColumn =
				gcnew System::Windows::Forms::DataGridViewTextBoxColumn();

			registrationColumn->HeaderText =
				L"Registration No.";

			registrationColumn->Name =
				L"RegistrationNumber";

			registrationColumn->Width = 165;

			this->studentsGrid->Columns->Add(
				registrationColumn
			);


			System::Windows::Forms::DataGridViewTextBoxColumn^ nameColumn =
				gcnew System::Windows::Forms::DataGridViewTextBoxColumn();

			nameColumn->HeaderText =
				L"Student Name";

			nameColumn->Name =
				L"StudentName";

			nameColumn->AutoSizeMode =
				System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;

			this->studentsGrid->Columns->Add(
				nameColumn
			);


			System::Windows::Forms::DataGridViewTextBoxColumn^ classColumn =
				gcnew System::Windows::Forms::DataGridViewTextBoxColumn();

			classColumn->HeaderText =
				L"Class";

			classColumn->Name =
				L"Class";

			classColumn->Width = 100;

			this->studentsGrid->Columns->Add(
				classColumn
			);


			System::Windows::Forms::DataGridViewTextBoxColumn^ streamColumn =
				gcnew System::Windows::Forms::DataGridViewTextBoxColumn();

			streamColumn->HeaderText =
				L"Stream";

			streamColumn->Name =
				L"Stream";

			streamColumn->Width = 110;

			this->studentsGrid->Columns->Add(
				streamColumn
			);


			System::Windows::Forms::DataGridViewTextBoxColumn^ statusColumn =
				gcnew System::Windows::Forms::DataGridViewTextBoxColumn();

			statusColumn->HeaderText =
				L"Status";

			statusColumn->Name =
				L"Status";

			statusColumn->Width = 100;

			this->studentsGrid->Columns->Add(
				statusColumn
			);


			// ============================================================
			// BOTTOM ACTIONS
			// ============================================================

			this->buttonPanel->Dock =
				System::Windows::Forms::DockStyle::Fill;

			this->buttonPanel->FlowDirection =
				System::Windows::Forms::FlowDirection::RightToLeft;

			this->buttonPanel->WrapContents =
				false;

			this->buttonPanel->Padding =
				System::Windows::Forms::Padding(
					0, 8, 0, 0
				);


			// View Profile
			this->btnViewProfile->Text =
				L"View Profile";

			this->btnViewProfile->Font =
				regularFont;

			this->btnViewProfile->Size =
				System::Drawing::Size(120, 40);

			this->btnViewProfile->Margin =
				System::Windows::Forms::Padding(
					8, 0, 0, 0
				);


			// Edit
			this->btnEditStudent->Text =
				L"Edit Student";

			this->btnEditStudent->Font =
				regularFont;

			this->btnEditStudent->Size =
				System::Drawing::Size(120, 40);

			this->btnEditStudent->Margin =
				System::Windows::Forms::Padding(
					8, 0, 0, 0
				);


			// Back
			this->btnBack->Text =
				L"Back to Dashboard";

			this->btnBack->Font =
				regularFont;

			this->btnBack->Size =
				System::Drawing::Size(150, 40);

			this->btnBack->Margin =
				System::Windows::Forms::Padding(
					8, 0, 0, 0
				);

			this->btnBack->Click +=
				gcnew System::EventHandler(
					this,
					&StudentManagement::btnBack_Click
				);


			this->buttonPanel->Controls->Add(
				this->btnBack
			);

			this->buttonPanel->Controls->Add(
				this->btnEditStudent
			);

			this->buttonPanel->Controls->Add(
				this->btnViewProfile
			);


			// ============================================================
			// MAIN FORM
			// ============================================================

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
				this->lblStudentCount,
				0,
				2
			);

			this->mainLayout->Controls->Add(
				this->studentsGrid,
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

			this->ResumeLayout(false);
		}

#pragma endregion
	};
}
