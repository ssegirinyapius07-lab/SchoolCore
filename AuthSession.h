#pragma once

using namespace System;
using namespace System::Collections::Generic;

namespace SchoolCore
{
    public ref class AuthSession abstract sealed
    {
    public:
        static int UserId = 0;
        static String^ Username = L"";
        static String^ FullName = L"";
        static String^ RoleName = L"";
        static System::Collections::Generic::HashSet<String^>^ Permissions =
            gcnew System::Collections::Generic::HashSet<String^>();

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
            return
                !String::IsNullOrWhiteSpace(permission) &&
                Permissions->Contains(permission);
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
