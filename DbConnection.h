#pragma once

#include <memory>
#include <mariadb/conncpp.hpp>

namespace SchoolCore
{
	class DbConnection
	{
	public:

		static std::unique_ptr<sql::Connection> GetConnection()
		{
			sql::Driver* driver =
				sql::mariadb::get_driver_instance();

			sql::SQLString url(
				"jdbc:mariadb://127.0.0.1:3306/schoolcore_db"
			);

			sql::Properties properties({
				{"user", "root"},
				{"password", ""}
				});

			std::unique_ptr<sql::Connection> connection(
				driver->connect(url, properties)
			);

			return connection;
		}
	};
}