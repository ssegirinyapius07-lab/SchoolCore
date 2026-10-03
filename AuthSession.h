#pragma once

namespace SchoolCore
{
    using namespace System;
    public ref class AuthSession abstract sealed
    {
    public:
        static int UserId = 0;
        static String^ Username = L"";
        static String^ FullName = L"";
        static String^ RoleName = L"";
        static System::Collections::Generic::List<String^>^ Permissions =
            gcnew System::Collections::Generic::List<String^>();

        static void Start(
            int userId,
            String^ username,
            String^ fullName,
            String^ roleName,
            System::Collections::IEnumerable^ permissions)
        {
            UserId = userId;
            Username = username;
            FullName = fullName;
            RoleName = roleName;

            Permissions->Clear();

            if (permissions != nullptr)
            {
                for each (Object^ item in permissions)
                {
                    String^ permission =
                        dynamic_cast<String^>(item);

                    if (!String::IsNullOrWhiteSpace(permission))
                    {
                        Permissions->Add(permission);
                    }
                }
            }
        }

        static bool HasPermission(String^ permission)
        {
            if (String::IsNullOrWhiteSpace(permission))
            {
                return false;
            }

            for each (String^ item in Permissions)
            {
                if (String::Equals(
                        item,
                        permission,
                        StringComparison::OrdinalIgnoreCase))
                {
                    return true;
                }
            }

            return false;
        }

        static void Clear()
        {
            UserId = 0;
            Username = L"";
            FullName = L"";
            RoleName = L"";
            Permissions->Clear();
        }
    };
}
