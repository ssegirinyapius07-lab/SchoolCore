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
        static HashSet<String^>^ Permissions =
            gcnew HashSet<String^>();

        static void Start(
            int userId,
            String^ username,
            String^ fullName,
            String^ roleName,
            IEnumerable<String^>^ permissions)
        {
            UserId = userId;
            Username = username;
            FullName = fullName;
            RoleName = roleName;

            Permissions->Clear();

            if (permissions != nullptr)
            {
                for each (String^ permission in permissions)
                {
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
