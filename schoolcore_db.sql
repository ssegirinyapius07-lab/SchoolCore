-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Oct 04, 2026 at 11:53 AM
-- Server version: 10.4.32-MariaDB
-- PHP Version: 8.2.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `schoolcore_db`
--
CREATE DATABASE IF NOT EXISTS `schoolcore_db` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE `schoolcore_db`;

-- --------------------------------------------------------

--
-- Table structure for table `academic_levels`
--
-- Creation: Oct 03, 2026 at 10:52 AM
--

DROP TABLE IF EXISTS `academic_levels`;
CREATE TABLE `academic_levels` (
  `academic_level_id` int(10) UNSIGNED NOT NULL,
  `level_code` varchar(30) NOT NULL,
  `level_name` varchar(100) NOT NULL,
  `description` varchar(255) DEFAULT NULL,
  `status` varchar(30) NOT NULL DEFAULT 'Active'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `academic_levels`
--

INSERT DELAYED IGNORE INTO `academic_levels` (`academic_level_id`, `level_code`, `level_name`, `description`, `status`) VALUES
(1, 'O_LEVEL', 'O-Level', 'Lower secondary academic level, Senior 1 to Senior 4.', 'Active'),
(2, 'A_LEVEL', 'A-Level', 'Advanced secondary academic level, Senior 5 to Senior 6.', 'Active');

-- --------------------------------------------------------

--
-- Table structure for table `academic_years`
--
-- Creation: Oct 03, 2026 at 02:11 PM
--

DROP TABLE IF EXISTS `academic_years`;
CREATE TABLE `academic_years` (
  `academic_year_id` int(10) UNSIGNED NOT NULL,
  `year_name` varchar(20) NOT NULL,
  `curriculum_id` int(10) UNSIGNED DEFAULT NULL,
  `start_date` date DEFAULT NULL,
  `end_date` date DEFAULT NULL,
  `status` enum('Active','Inactive') NOT NULL DEFAULT 'Active'
) ;

--
-- Dumping data for table `academic_years`
--

INSERT DELAYED IGNORE INTO `academic_years` (`academic_year_id`, `year_name`, `curriculum_id`, `start_date`, `end_date`, `status`) VALUES
(3, '2026', NULL, NULL, NULL, 'Active');

-- --------------------------------------------------------

--
-- Table structure for table `attendance_records`
--
-- Creation: Oct 03, 2026 at 08:42 AM
--

DROP TABLE IF EXISTS `attendance_records`;
CREATE TABLE `attendance_records` (
  `attendance_record_id` int(10) UNSIGNED NOT NULL,
  `attendance_session_id` int(10) UNSIGNED NOT NULL,
  `student_id` int(10) UNSIGNED NOT NULL,
  `attendance_status` varchar(30) NOT NULL,
  `remarks` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `attendance_sessions`
--
-- Creation: Oct 02, 2026 at 11:11 AM
--

DROP TABLE IF EXISTS `attendance_sessions`;
CREATE TABLE `attendance_sessions` (
  `attendance_session_id` int(10) UNSIGNED NOT NULL,
  `attendance_date` date NOT NULL,
  `academic_year_id` int(10) UNSIGNED NOT NULL,
  `term_id` int(10) UNSIGNED NOT NULL,
  `class_id` int(10) UNSIGNED NOT NULL,
  `stream_id` int(10) UNSIGNED DEFAULT NULL,
  `recorded_by` int(10) UNSIGNED DEFAULT NULL,
  `remarks` varchar(255) DEFAULT NULL,
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `audit_logs`
--
-- Creation: Oct 02, 2026 at 11:11 AM
--

DROP TABLE IF EXISTS `audit_logs`;
CREATE TABLE `audit_logs` (
  `audit_log_id` bigint(20) UNSIGNED NOT NULL,
  `user_id` int(10) UNSIGNED DEFAULT NULL,
  `action` varchar(100) NOT NULL,
  `table_name` varchar(100) DEFAULT NULL,
  `record_id` varchar(100) DEFAULT NULL,
  `description` text DEFAULT NULL,
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `classes`
--
-- Creation: Oct 03, 2026 at 10:52 AM
--

DROP TABLE IF EXISTS `classes`;
CREATE TABLE `classes` (
  `class_id` int(10) UNSIGNED NOT NULL,
  `class_name` varchar(50) NOT NULL,
  `academic_level_id` int(10) UNSIGNED NOT NULL,
  `description` varchar(255) DEFAULT NULL,
  `status` enum('Active','Inactive') NOT NULL DEFAULT 'Active'
) ;

--
-- Dumping data for table `classes`
--

INSERT DELAYED IGNORE INTO `classes` (`class_id`, `class_name`, `academic_level_id`, `description`, `status`) VALUES
(1, 'Senior 1', 1, NULL, 'Active'),
(2, 'Senior 2', 1, NULL, 'Active'),
(3, 'Senior 3', 1, NULL, 'Active'),
(4, 'Senior 4', 1, NULL, 'Active'),
(5, 'Senior 5', 2, NULL, 'Active'),
(6, 'Senior 6', 2, NULL, 'Active');

-- --------------------------------------------------------

--
-- Table structure for table `class_subjects`
--
-- Creation: Oct 02, 2026 at 11:11 AM
--

DROP TABLE IF EXISTS `class_subjects`;
CREATE TABLE `class_subjects` (
  `class_subject_id` int(10) UNSIGNED NOT NULL,
  `academic_year_id` int(10) UNSIGNED NOT NULL,
  `class_id` int(10) UNSIGNED NOT NULL,
  `stream_id` int(10) UNSIGNED DEFAULT NULL,
  `subject_id` int(10) UNSIGNED NOT NULL,
  `status` varchar(30) NOT NULL DEFAULT 'Active'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `combination_subjects`
--
-- Creation: Oct 03, 2026 at 10:52 AM
--

DROP TABLE IF EXISTS `combination_subjects`;
CREATE TABLE `combination_subjects` (
  `combination_subject_id` int(10) UNSIGNED NOT NULL,
  `combination_id` int(10) UNSIGNED NOT NULL,
  `subject_id` int(10) UNSIGNED NOT NULL,
  `subject_role` varchar(30) NOT NULL DEFAULT 'Principal',
  `status` varchar(30) NOT NULL DEFAULT 'Active'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `combination_subjects`
--

INSERT DELAYED IGNORE INTO `combination_subjects` (`combination_subject_id`, `combination_id`, `subject_id`, `subject_role`, `status`) VALUES
(1, 1, 4, 'Principal', 'Active'),
(2, 1, 8, 'Principal', 'Active'),
(3, 1, 1, 'Principal', 'Active'),
(4, 1, 9, 'Subsidiary', 'Active');

-- --------------------------------------------------------

--
-- Table structure for table `curricula`
--
-- Creation: Oct 03, 2026 at 02:11 PM
--

DROP TABLE IF EXISTS `curricula`;
CREATE TABLE `curricula` (
  `curriculum_id` int(10) UNSIGNED NOT NULL,
  `curriculum_code` varchar(50) NOT NULL,
  `curriculum_name` varchar(150) NOT NULL,
  `academic_level_id` int(10) UNSIGNED NOT NULL,
  `effective_from_year` int(11) DEFAULT NULL,
  `effective_to_year` int(11) DEFAULT NULL,
  `description` varchar(255) DEFAULT NULL,
  `status` varchar(30) NOT NULL DEFAULT 'Active'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `curricula`
--

INSERT DELAYED IGNORE INTO `curricula` (`curriculum_id`, `curriculum_code`, `curriculum_name`, `academic_level_id`, `effective_from_year`, `effective_to_year`, `description`, `status`) VALUES
(1, 'UCE_CBC', 'UCE Competency Based Curriculum', 1, NULL, NULL, 'Current UCE curriculum used for the new lower secondary assessment structure.', 'Active'),
(2, 'UACE_ALIGNED', 'UACE Aligned Curriculum', 2, NULL, NULL, 'Current UACE aligned curriculum assessment structure.', 'Active');

-- --------------------------------------------------------

--
-- Table structure for table `curriculum_subjects`
--
-- Creation: Oct 03, 2026 at 11:25 PM
--

DROP TABLE IF EXISTS `curriculum_subjects`;
CREATE TABLE `curriculum_subjects` (
  `curriculum_subject_id` int(10) UNSIGNED NOT NULL,
  `curriculum_id` int(10) UNSIGNED NOT NULL,
  `subject_id` int(10) UNSIGNED NOT NULL,
  `uneb_subject_code` varchar(30) NOT NULL,
  `subject_group` varchar(60) NOT NULL DEFAULT 'Other',
  `requirement_type` varchar(20) NOT NULL DEFAULT 'Optional',
  `status` varchar(30) NOT NULL DEFAULT 'Active'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `curriculum_subjects`
--

INSERT DELAYED IGNORE INTO `curriculum_subjects` (`curriculum_subject_id`, `curriculum_id`, `subject_id`, `uneb_subject_code`, `subject_group`, `requirement_type`, `status`) VALUES
(1, 2, 1, 'P425', 'Sciences', 'Optional', 'Active'),
(2, 1, 4, '245', 'Sciences', 'Mandatory', 'Active');

-- --------------------------------------------------------

--
-- Table structure for table `discipline_records`
--
-- Creation: Oct 02, 2026 at 11:11 AM
--

DROP TABLE IF EXISTS `discipline_records`;
CREATE TABLE `discipline_records` (
  `discipline_record_id` int(10) UNSIGNED NOT NULL,
  `student_id` int(10) UNSIGNED NOT NULL,
  `incident_date` date NOT NULL,
  `category` varchar(100) DEFAULT NULL,
  `description` text NOT NULL,
  `action_taken` text DEFAULT NULL,
  `follow_up_date` date DEFAULT NULL,
  `status` varchar(30) NOT NULL DEFAULT 'Open',
  `recorded_by` int(10) UNSIGNED DEFAULT NULL,
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `enrollments`
--
-- Creation: Oct 03, 2026 at 10:52 AM
--

DROP TABLE IF EXISTS `enrollments`;
CREATE TABLE `enrollments` (
  `enrollment_id` int(10) UNSIGNED NOT NULL,
  `student_id` int(10) UNSIGNED NOT NULL,
  `academic_year_id` int(10) UNSIGNED NOT NULL,
  `term_id` int(10) UNSIGNED NOT NULL,
  `class_id` int(10) UNSIGNED NOT NULL,
  `stream_id` int(10) UNSIGNED DEFAULT NULL,
  `combination_id` int(10) UNSIGNED DEFAULT NULL,
  `enrollment_date` date DEFAULT NULL,
  `status` varchar(30) NOT NULL DEFAULT 'Active'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `enrollments`
--

INSERT DELAYED IGNORE INTO `enrollments` (`enrollment_id`, `student_id`, `academic_year_id`, `term_id`, `class_id`, `stream_id`, `combination_id`, `enrollment_date`, `status`) VALUES
(1, 1, 3, 1, 1, 1, NULL, '2026-10-02', 'Active'),
(2, 2, 3, 1, 5, 2, NULL, '2026-10-04', 'Active'),
(3, 3, 3, 1, 2, 8, NULL, '2026-10-04', 'Active');

--
-- Triggers `enrollments`
--
DROP TRIGGER IF EXISTS `trg_enrollments_validate_stream_insert`;
DELIMITER $$
CREATE TRIGGER `trg_enrollments_validate_stream_insert` BEFORE INSERT ON `enrollments` FOR EACH ROW BEGIN
    DECLARE classLevelCode VARCHAR(30);
    DECLARE streamClassId INT UNSIGNED;
    DECLARE streamName VARCHAR(100);

    IF NEW.stream_id IS NOT NULL THEN
        SELECT
            cLevel.level_code,
            s.class_id,
            s.stream_name
        INTO
            classLevelCode,
            streamClassId,
            streamName
        FROM streams s
        INNER JOIN classes c
            ON c.class_id = s.class_id
        INNER JOIN academic_levels cLevel
            ON cLevel.academic_level_id = c.academic_level_id
        WHERE s.stream_id = NEW.stream_id
        LIMIT 1;

        IF streamClassId IS NULL THEN
            SIGNAL SQLSTATE '45000'
                SET MESSAGE_TEXT = 'The selected stream does not exist.';
        ELSEIF streamClassId <> NEW.class_id THEN
            SIGNAL SQLSTATE '45000'
                SET MESSAGE_TEXT = 'The selected stream does not belong to the selected class.';
        ELSEIF classLevelCode = 'O_LEVEL'
            AND streamName NOT IN ('West', 'South', 'East', 'North') THEN
            SIGNAL SQLSTATE '45000'
                SET MESSAGE_TEXT = 'O-Level students can only use West, South, East or North.';
        ELSEIF classLevelCode = 'A_LEVEL'
            AND streamName NOT IN ('Sciences', 'Arts') THEN
            SIGNAL SQLSTATE '45000'
                SET MESSAGE_TEXT = 'A-Level students can only use Sciences or Arts.';
        END IF;
    END IF;
END
$$
DELIMITER ;
DROP TRIGGER IF EXISTS `trg_enrollments_validate_stream_update`;
DELIMITER $$
CREATE TRIGGER `trg_enrollments_validate_stream_update` BEFORE UPDATE ON `enrollments` FOR EACH ROW BEGIN
    DECLARE classLevelCode VARCHAR(30);
    DECLARE streamClassId INT UNSIGNED;
    DECLARE streamName VARCHAR(100);

    IF NEW.stream_id IS NOT NULL THEN
        SELECT
            cLevel.level_code,
            s.class_id,
            s.stream_name
        INTO
            classLevelCode,
            streamClassId,
            streamName
        FROM streams s
        INNER JOIN classes c
            ON c.class_id = s.class_id
        INNER JOIN academic_levels cLevel
            ON cLevel.academic_level_id = c.academic_level_id
        WHERE s.stream_id = NEW.stream_id
        LIMIT 1;

        IF streamClassId IS NULL THEN
            SIGNAL SQLSTATE '45000'
                SET MESSAGE_TEXT = 'The selected stream does not exist.';
        ELSEIF streamClassId <> NEW.class_id THEN
            SIGNAL SQLSTATE '45000'
                SET MESSAGE_TEXT = 'The selected stream does not belong to the selected class.';
        ELSEIF classLevelCode = 'O_LEVEL'
            AND streamName NOT IN ('West', 'South', 'East', 'North') THEN
            SIGNAL SQLSTATE '45000'
                SET MESSAGE_TEXT = 'O-Level students can only use West, South, East or North.';
        ELSEIF classLevelCode = 'A_LEVEL'
            AND streamName NOT IN ('Sciences', 'Arts') THEN
            SIGNAL SQLSTATE '45000'
                SET MESSAGE_TEXT = 'A-Level students can only use Sciences or Arts.';
        END IF;
    END IF;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `enrollment_optional_subjects`
--
-- Creation: Oct 03, 2026 at 11:22 PM
--

DROP TABLE IF EXISTS `enrollment_optional_subjects`;
CREATE TABLE `enrollment_optional_subjects` (
  `enrollment_optional_subject_id` int(10) UNSIGNED NOT NULL,
  `enrollment_id` int(10) UNSIGNED NOT NULL,
  `subject_id` int(10) UNSIGNED NOT NULL,
  `option_number` tinyint(3) UNSIGNED NOT NULL,
  `selected_date` date DEFAULT NULL,
  `status` varchar(30) NOT NULL DEFAULT 'Active'
) ;

-- --------------------------------------------------------

--
-- Table structure for table `examinations`
--
-- Creation: Oct 02, 2026 at 11:11 AM
--

DROP TABLE IF EXISTS `examinations`;
CREATE TABLE `examinations` (
  `examination_id` int(10) UNSIGNED NOT NULL,
  `academic_year_id` int(10) UNSIGNED NOT NULL,
  `term_id` int(10) UNSIGNED NOT NULL,
  `class_id` int(10) UNSIGNED NOT NULL,
  `examination_name` varchar(100) NOT NULL,
  `examination_type` varchar(50) DEFAULT NULL,
  `start_date` date DEFAULT NULL,
  `end_date` date DEFAULT NULL,
  `status` varchar(30) NOT NULL DEFAULT 'Draft',
  `approved_by` int(10) UNSIGNED DEFAULT NULL,
  `approved_at` datetime DEFAULT NULL,
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `examinations`
--

INSERT DELAYED IGNORE INTO `examinations` (`examination_id`, `academic_year_id`, `term_id`, `class_id`, `examination_name`, `examination_type`, `start_date`, `end_date`, `status`, `approved_by`, `approved_at`, `created_at`) VALUES
(1, 3, 1, 1, 'Mid Term', 'Beginning / Mid', '2026-10-04', '2026-10-08', 'Draft', NULL, NULL, '2026-10-04 01:43:35');

-- --------------------------------------------------------

--
-- Table structure for table `examination_papers`
--
-- Creation: Oct 03, 2026 at 01:47 PM
--

DROP TABLE IF EXISTS `examination_papers`;
CREATE TABLE `examination_papers` (
  `examination_paper_id` int(10) UNSIGNED NOT NULL,
  `examination_subject_id` int(10) UNSIGNED NOT NULL,
  `paper_id` int(10) UNSIGNED NOT NULL,
  `max_score` decimal(10,2) NOT NULL DEFAULT 100.00,
  `pass_mark` decimal(10,2) DEFAULT NULL,
  `status` varchar(30) NOT NULL DEFAULT 'Active'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `examination_subjects`
--
-- Creation: Oct 03, 2026 at 10:52 AM
--

DROP TABLE IF EXISTS `examination_subjects`;
CREATE TABLE `examination_subjects` (
  `examination_subject_id` int(10) UNSIGNED NOT NULL,
  `examination_id` int(10) UNSIGNED NOT NULL,
  `subject_id` int(10) UNSIGNED NOT NULL,
  `max_score` decimal(6,2) NOT NULL,
  `pass_mark` decimal(6,2) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `fee_charges`
--
-- Creation: Oct 02, 2026 at 11:11 AM
--

DROP TABLE IF EXISTS `fee_charges`;
CREATE TABLE `fee_charges` (
  `fee_charge_id` int(10) UNSIGNED NOT NULL,
  `student_id` int(10) UNSIGNED NOT NULL,
  `fee_structure_id` int(10) UNSIGNED NOT NULL,
  `amount` decimal(12,2) NOT NULL,
  `due_date` date DEFAULT NULL,
  `status` varchar(30) NOT NULL DEFAULT 'Unpaid',
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `fee_structures`
--
-- Creation: Oct 02, 2026 at 11:11 AM
--

DROP TABLE IF EXISTS `fee_structures`;
CREATE TABLE `fee_structures` (
  `fee_structure_id` int(10) UNSIGNED NOT NULL,
  `academic_year_id` int(10) UNSIGNED NOT NULL,
  `term_id` int(10) UNSIGNED NOT NULL,
  `class_id` int(10) UNSIGNED DEFAULT NULL,
  `stream_id` int(10) UNSIGNED DEFAULT NULL,
  `fee_name` varchar(150) NOT NULL,
  `amount` decimal(12,2) NOT NULL,
  `due_date` date DEFAULT NULL,
  `description` varchar(255) DEFAULT NULL,
  `status` varchar(30) NOT NULL DEFAULT 'Active'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `grading_scales`
--
-- Creation: Oct 04, 2026 at 01:31 AM
--

DROP TABLE IF EXISTS `grading_scales`;
CREATE TABLE `grading_scales` (
  `grading_scale_id` int(10) UNSIGNED NOT NULL,
  `academic_level_id` int(10) UNSIGNED DEFAULT NULL,
  `scale_name` varchar(100) NOT NULL,
  `min_score` decimal(6,2) NOT NULL,
  `max_score` decimal(6,2) NOT NULL,
  `grade` varchar(20) NOT NULL,
  `grade_point` decimal(6,2) DEFAULT NULL,
  `remarks` varchar(255) DEFAULT NULL,
  `status` varchar(30) NOT NULL DEFAULT 'Active'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `grading_scales`
--

INSERT DELAYED IGNORE INTO `grading_scales` (`grading_scale_id`, `academic_level_id`, `scale_name`, `min_score`, `max_score`, `grade`, `grade_point`, `remarks`, `status`) VALUES
(1, 1, 'O-Level Standard', 80.00, 100.00, 'A', NULL, 'Exceptional', 'Active'),
(3, 1, 'O-Level Standard', 70.00, 79.99, 'B', NULL, 'Outstanding', 'Active'),
(4, 1, 'O-Level Standard', 60.00, 69.99, 'C', NULL, 'Satisfactory', 'Active'),
(5, 1, 'O-Level Standard', 50.00, 59.99, 'D', NULL, 'Basic', 'Active'),
(6, 1, 'O-Level Standard', 0.00, 49.99, 'E', NULL, 'Elementary', 'Active'),
(7, 2, 'A-Level Standard', 80.00, 100.00, 'A', 6.00, 'Excellent', 'Active'),
(8, 2, 'A-Level Standard', 70.00, 79.99, 'B', 5.00, 'Very Good', 'Active'),
(9, 2, 'A-Level Standard', 60.00, 69.99, 'C', 4.00, 'Good', 'Active'),
(10, 2, 'A-Level Standard', 50.00, 59.99, 'D', 3.00, 'Satisfactory', 'Active'),
(11, 2, 'A-Level Standard', 40.00, 49.99, 'E', 2.00, 'Pass', 'Active'),
(12, 2, 'A-Level Standard', 30.00, 39.99, 'O', 1.00, 'Ordinary', 'Active'),
(13, 2, 'A-Level Standard', 0.00, 29.99, 'F', 0.00, 'Fail', 'Active');

-- --------------------------------------------------------

--
-- Table structure for table `guardians`
--
-- Creation: Oct 02, 2026 at 11:11 AM
--

DROP TABLE IF EXISTS `guardians`;
CREATE TABLE `guardians` (
  `guardian_id` int(10) UNSIGNED NOT NULL,
  `full_name` varchar(150) NOT NULL,
  `relationship` varchar(50) DEFAULT NULL,
  `phone_number` varchar(30) NOT NULL,
  `alternative_phone` varchar(30) DEFAULT NULL,
  `email` varchar(150) DEFAULT NULL,
  `address` varchar(255) DEFAULT NULL,
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `guardians`
--

INSERT DELAYED IGNORE INTO `guardians` (`guardian_id`, `full_name`, `relationship`, `phone_number`, `alternative_phone`, `email`, `address`, `created_at`) VALUES
(1, 'NASANGA PATIENCE', 'MOTHER', '0789777767', '', '', 'KAYUNGA', '2026-10-02 14:40:59'),
(2, 'Lule Paul', 'Father', '0767268968', '', '', 'Mbale', '2026-10-03 21:43:35'),
(3, 'Lubwama Paul', 'Father', '0786565665', '', '', 'Kasokwe', '2026-10-04 00:23:43');

-- --------------------------------------------------------

--
-- Table structure for table `marks`
--
-- Creation: Oct 04, 2026 at 01:05 AM
--

DROP TABLE IF EXISTS `marks`;
CREATE TABLE `marks` (
  `mark_id` int(10) UNSIGNED NOT NULL,
  `examination_subject_id` int(10) UNSIGNED NOT NULL,
  `examination_paper_id` int(10) UNSIGNED DEFAULT NULL,
  `student_id` int(10) UNSIGNED NOT NULL,
  `score` decimal(6,2) NOT NULL,
  `grade` varchar(20) DEFAULT NULL,
  `grade_point` decimal(6,2) DEFAULT NULL,
  `remarks` varchar(255) DEFAULT NULL,
  `entered_by` int(10) UNSIGNED DEFAULT NULL,
  `entered_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `notices`
--
-- Creation: Oct 02, 2026 at 11:11 AM
--

DROP TABLE IF EXISTS `notices`;
CREATE TABLE `notices` (
  `notice_id` int(10) UNSIGNED NOT NULL,
  `title` varchar(200) NOT NULL,
  `content` text NOT NULL,
  `audience` varchar(100) DEFAULT NULL,
  `publish_from` datetime DEFAULT NULL,
  `publish_until` datetime DEFAULT NULL,
  `status` varchar(30) NOT NULL DEFAULT 'Draft',
  `created_by` int(10) UNSIGNED DEFAULT NULL,
  `created_at` timestamp NOT NULL DEFAULT current_timestamp(),
  `updated_at` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `payments`
--
-- Creation: Oct 02, 2026 at 11:11 AM
--

DROP TABLE IF EXISTS `payments`;
CREATE TABLE `payments` (
  `payment_id` int(10) UNSIGNED NOT NULL,
  `student_id` int(10) UNSIGNED NOT NULL,
  `receipt_number` varchar(50) NOT NULL,
  `payment_date` date NOT NULL,
  `amount` decimal(12,2) NOT NULL,
  `payment_method` varchar(50) DEFAULT NULL,
  `transaction_reference` varchar(100) DEFAULT NULL,
  `received_by` int(10) UNSIGNED DEFAULT NULL,
  `remarks` varchar(255) DEFAULT NULL,
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `payment_allocations`
--
-- Creation: Oct 02, 2026 at 11:11 AM
--

DROP TABLE IF EXISTS `payment_allocations`;
CREATE TABLE `payment_allocations` (
  `payment_allocation_id` int(10) UNSIGNED NOT NULL,
  `payment_id` int(10) UNSIGNED NOT NULL,
  `fee_charge_id` int(10) UNSIGNED NOT NULL,
  `amount` decimal(12,2) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `permissions`
--
-- Creation: Oct 02, 2026 at 11:11 AM
--

DROP TABLE IF EXISTS `permissions`;
CREATE TABLE `permissions` (
  `permission_id` int(10) UNSIGNED NOT NULL,
  `permission_name` varchar(100) NOT NULL,
  `description` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `permissions`
--

INSERT DELAYED IGNORE INTO `permissions` (`permission_id`, `permission_name`, `description`) VALUES
(1, 'dashboard.view', 'View the main SchoolCore dashboard'),
(2, 'students.view', 'View student records'),
(3, 'students.manage', 'Register, edit, activate and manage students'),
(4, 'teachers.view', 'View teacher records'),
(5, 'teachers.manage', 'Register, edit, activate and manage teachers'),
(6, 'classes.view', 'View classes and streams'),
(7, 'classes.manage', 'Create and manage classes and streams'),
(8, 'subjects.view', 'View subjects'),
(9, 'subjects.manage', 'Create and manage subjects'),
(10, 'academic_years.view', 'View academic years and terms'),
(11, 'academic_years.manage', 'Create and manage academic years and terms'),
(12, 'timetable.view', 'View the school timetable'),
(13, 'timetable.manage', 'Create and manage timetable entries'),
(14, 'attendance.view', 'View attendance records'),
(15, 'attendance.manage', 'Record and manage attendance'),
(16, 'examinations.view', 'View examinations'),
(17, 'examinations.manage', 'Create and manage examinations'),
(18, 'fees.view', 'View fee and payment records'),
(19, 'fees.manage', 'Manage fees, charges and payments'),
(20, 'discipline.view', 'View discipline records'),
(21, 'discipline.manage', 'Create and manage discipline records'),
(22, 'communication.view', 'View school communication and notices'),
(23, 'communication.manage', 'Create and manage school communication'),
(24, 'reports.view', 'View system reports'),
(25, 'users.view', 'View system users'),
(26, 'users.manage', 'Create and manage system users and roles'),
(27, 'settings.view', 'View system settings'),
(28, 'settings.manage', 'Manage system settings');

-- --------------------------------------------------------

--
-- Table structure for table `roles`
--
-- Creation: Oct 02, 2026 at 11:02 AM
--

DROP TABLE IF EXISTS `roles`;
CREATE TABLE `roles` (
  `role_id` int(10) UNSIGNED NOT NULL,
  `role_name` varchar(50) NOT NULL,
  `description` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `roles`
--

INSERT DELAYED IGNORE INTO `roles` (`role_id`, `role_name`, `description`) VALUES
(1, 'Administrator', 'Full system administration'),
(2, 'Academic Registrar', 'Manages academic and student records'),
(3, 'Bursar', 'Manages fees and financial records'),
(4, 'Head Teacher', 'Supervises school operations and records');

-- --------------------------------------------------------

--
-- Table structure for table `role_permissions`
--
-- Creation: Oct 02, 2026 at 11:11 AM
--

DROP TABLE IF EXISTS `role_permissions`;
CREATE TABLE `role_permissions` (
  `role_id` int(10) UNSIGNED NOT NULL,
  `permission_id` int(10) UNSIGNED NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `role_permissions`
--

INSERT DELAYED IGNORE INTO `role_permissions` (`role_id`, `permission_id`) VALUES
(1, 1),
(1, 2),
(1, 3),
(1, 4),
(1, 5),
(1, 6),
(1, 7),
(1, 8),
(1, 9),
(1, 10),
(1, 11),
(1, 12),
(1, 13),
(1, 14),
(1, 15),
(1, 16),
(1, 17),
(1, 18),
(1, 19),
(1, 20),
(1, 21),
(1, 22),
(1, 23),
(1, 24),
(1, 25),
(1, 26),
(1, 27),
(1, 28),
(2, 1),
(2, 2),
(2, 3),
(2, 4),
(2, 6),
(2, 7),
(2, 8),
(2, 9),
(2, 10),
(2, 11),
(2, 12),
(2, 14),
(2, 16),
(2, 17),
(2, 20),
(2, 22),
(2, 24),
(3, 1),
(3, 2),
(3, 18),
(3, 19),
(3, 24),
(4, 1),
(4, 2),
(4, 4),
(4, 5),
(4, 6),
(4, 7),
(4, 8),
(4, 9),
(4, 10),
(4, 12),
(4, 13),
(4, 14),
(4, 15),
(4, 16),
(4, 17),
(4, 18),
(4, 20),
(4, 21),
(4, 22),
(4, 23),
(4, 24);

-- --------------------------------------------------------

--
-- Table structure for table `school_settings`
--
-- Creation: Oct 02, 2026 at 11:11 AM
--

DROP TABLE IF EXISTS `school_settings`;
CREATE TABLE `school_settings` (
  `setting_id` int(10) UNSIGNED NOT NULL,
  `setting_key` varchar(100) NOT NULL,
  `setting_value` text DEFAULT NULL,
  `updated_at` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `streams`
--
-- Creation: Oct 02, 2026 at 11:11 AM
--

DROP TABLE IF EXISTS `streams`;
CREATE TABLE `streams` (
  `stream_id` int(10) UNSIGNED NOT NULL,
  `class_id` int(10) UNSIGNED NOT NULL,
  `stream_name` varchar(50) NOT NULL,
  `description` varchar(255) DEFAULT NULL,
  `status` varchar(30) NOT NULL DEFAULT 'Active'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `streams`
--

INSERT DELAYED IGNORE INTO `streams` (`stream_id`, `class_id`, `stream_name`, `description`, `status`) VALUES
(1, 1, 'West', NULL, 'Active'),
(2, 5, 'Sciences', NULL, 'Active'),
(3, 5, 'Arts', NULL, 'Active'),
(4, 6, 'Sciences', NULL, 'Active'),
(5, 6, 'Arts', NULL, 'Active'),
(6, 2, 'South', NULL, 'Active'),
(7, 1, 'South', NULL, 'Active'),
(8, 2, 'East', NULL, 'Active'),
(9, 3, 'West', NULL, 'Active'),
(10, 4, 'South', NULL, 'Active');

--
-- Triggers `streams`
--
DROP TRIGGER IF EXISTS `trg_streams_validate_level_insert`;
DELIMITER $$
CREATE TRIGGER `trg_streams_validate_level_insert` BEFORE INSERT ON `streams` FOR EACH ROW BEGIN
    DECLARE levelCode VARCHAR(30);

    SELECT al.level_code
    INTO levelCode
    FROM classes c
    INNER JOIN academic_levels al
        ON al.academic_level_id = c.academic_level_id
    WHERE c.class_id = NEW.class_id
    LIMIT 1;

    IF levelCode IS NULL THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'The selected class does not have a valid academic level.';
    ELSEIF levelCode = 'O_LEVEL'
        AND NEW.stream_name NOT IN ('West', 'South', 'East', 'North') THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'O-Level streams must be West, South, East or North.';
    ELSEIF levelCode = 'A_LEVEL'
        AND NEW.stream_name NOT IN ('Sciences', 'Arts') THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'A-Level streams must be Sciences or Arts.';
    END IF;
END
$$
DELIMITER ;
DROP TRIGGER IF EXISTS `trg_streams_validate_level_update`;
DELIMITER $$
CREATE TRIGGER `trg_streams_validate_level_update` BEFORE UPDATE ON `streams` FOR EACH ROW BEGIN
    DECLARE levelCode VARCHAR(30);

    SELECT al.level_code
    INTO levelCode
    FROM classes c
    INNER JOIN academic_levels al
        ON al.academic_level_id = c.academic_level_id
    WHERE c.class_id = NEW.class_id
    LIMIT 1;

    IF levelCode IS NULL THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'The selected class does not have a valid academic level.';
    ELSEIF levelCode = 'O_LEVEL'
        AND NEW.stream_name NOT IN ('West', 'South', 'East', 'North') THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'O-Level streams must be West, South, East or North.';
    ELSEIF levelCode = 'A_LEVEL'
        AND NEW.stream_name NOT IN ('Sciences', 'Arts') THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'A-Level streams must be Sciences or Arts.';
    END IF;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `students`
--
-- Creation: Oct 02, 2026 at 02:40 PM
--

DROP TABLE IF EXISTS `students`;
CREATE TABLE `students` (
  `student_id` int(10) UNSIGNED NOT NULL,
  `registration_number` varchar(50) NOT NULL,
  `registration_year` smallint(5) UNSIGNED DEFAULT NULL,
  `registration_sequence` int(10) UNSIGNED DEFAULT NULL,
  `first_name` varchar(100) NOT NULL,
  `middle_name` varchar(100) DEFAULT NULL,
  `last_name` varchar(100) NOT NULL,
  `date_of_birth` date DEFAULT NULL,
  `gender` varchar(30) DEFAULT NULL,
  `admission_date` date DEFAULT NULL,
  `home_address` varchar(255) DEFAULT NULL,
  `photo_path` varchar(500) DEFAULT NULL,
  `status` varchar(30) NOT NULL DEFAULT 'Active',
  `created_at` timestamp NOT NULL DEFAULT current_timestamp(),
  `updated_at` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `students`
--

INSERT DELAYED IGNORE INTO `students` (`student_id`, `registration_number`, `registration_year`, `registration_sequence`, `first_name`, `middle_name`, `last_name`, `date_of_birth`, `gender`, `admission_date`, `home_address`, `photo_path`, `status`, `created_at`, `updated_at`) VALUES
(1, 'STU/2026/0001', 2026, 1, 'PIUS', '', 'PIO', '2005-11-21', 'Male', '2026-10-02', 'KAYUNGA', 'StudentPhotos\\STU_2026_0001.jpg', 'Active', '2026-10-02 14:40:59', '2026-10-03 01:48:09'),
(2, 'STU/2026/0002', 2026, 2, 'Ssembidde', '', 'Pius', '2002-02-15', 'Male', '2026-10-04', 'Mbale Town', 'StudentPhotos\\STU_2026_0002.jpg', 'Active', '2026-10-03 21:43:35', '2026-10-03 21:43:35'),
(3, 'STU/2026/0003', 2026, 3, 'Lubwama', '', 'Tom', '2005-07-09', 'Male', '2026-10-04', 'Kasokwe', 'StudentPhotos\\STU_2026_0003.jpg', 'Active', '2026-10-04 00:23:43', '2026-10-04 00:33:26');

-- --------------------------------------------------------

--
-- Table structure for table `student_guardians`
--
-- Creation: Oct 02, 2026 at 11:11 AM
--

DROP TABLE IF EXISTS `student_guardians`;
CREATE TABLE `student_guardians` (
  `student_id` int(10) UNSIGNED NOT NULL,
  `guardian_id` int(10) UNSIGNED NOT NULL,
  `is_primary` tinyint(1) NOT NULL DEFAULT 0,
  `relationship_note` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `student_guardians`
--

INSERT DELAYED IGNORE INTO `student_guardians` (`student_id`, `guardian_id`, `is_primary`, `relationship_note`) VALUES
(1, 1, 1, NULL),
(2, 2, 1, NULL),
(3, 3, 1, NULL);

-- --------------------------------------------------------

--
-- Table structure for table `student_registration_sequences`
--
-- Creation: Oct 02, 2026 at 02:27 PM
--

DROP TABLE IF EXISTS `student_registration_sequences`;
CREATE TABLE `student_registration_sequences` (
  `registration_year` smallint(5) UNSIGNED NOT NULL,
  `last_sequence` int(10) UNSIGNED NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `student_registration_sequences`
--

INSERT DELAYED IGNORE INTO `student_registration_sequences` (`registration_year`, `last_sequence`) VALUES
(2026, 3);

-- --------------------------------------------------------

--
-- Table structure for table `student_subject_results`
--
-- Creation: Oct 04, 2026 at 01:24 AM
--

DROP TABLE IF EXISTS `student_subject_results`;
CREATE TABLE `student_subject_results` (
  `student_subject_result_id` int(10) UNSIGNED NOT NULL,
  `enrollment_id` int(10) UNSIGNED NOT NULL,
  `examination_subject_id` int(10) UNSIGNED NOT NULL,
  `total_score` decimal(10,2) NOT NULL DEFAULT 0.00,
  `total_max_score` decimal(10,2) NOT NULL DEFAULT 0.00,
  `percentage` decimal(6,2) NOT NULL DEFAULT 0.00,
  `grade` varchar(20) DEFAULT NULL,
  `grade_point` decimal(6,2) DEFAULT NULL,
  `remarks` varchar(255) DEFAULT NULL,
  `paper_count` int(10) UNSIGNED NOT NULL DEFAULT 0,
  `status` varchar(30) NOT NULL DEFAULT 'Calculated',
  `calculated_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `subjects`
--
-- Creation: Oct 03, 2026 at 01:59 PM
--

DROP TABLE IF EXISTS `subjects`;
CREATE TABLE `subjects` (
  `subject_id` int(10) UNSIGNED NOT NULL,
  `subject_code` varchar(100) DEFAULT NULL,
  `subject_name` varchar(100) NOT NULL,
  `description` varchar(255) DEFAULT NULL,
  `status` varchar(30) NOT NULL DEFAULT 'Active'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `subjects`
--

INSERT DELAYED IGNORE INTO `subjects` (`subject_id`, `subject_code`, `subject_name`, `description`, `status`) VALUES
(1, NULL, 'Mathematics', 'mathematics', 'Active'),
(2, NULL, 'Physics', 'Both Thoery and practical', 'Active'),
(3, NULL, 'Economics', 'Business subject', 'Active'),
(4, NULL, 'Biology', 'Science Subject', 'Active'),
(5, NULL, 'English Language', 'Grammer and Comprehesion', 'Active'),
(6, NULL, 'Geography', 'World Geography', 'Active'),
(7, NULL, 'Literature in English', 'Book Reading', 'Active'),
(8, NULL, 'Chemistry', 'Chemicals', 'Active'),
(9, NULL, 'Subsidiary ICT', 'Technology', 'Active'),
(10, NULL, 'History & Political Education', NULL, 'Active'),
(11, NULL, 'Kiswahili', NULL, 'Active'),
(12, NULL, 'Physical Education', NULL, 'Active'),
(13, NULL, 'Religious Education', NULL, 'Active'),
(14, NULL, 'Entrepreneurship', NULL, 'Active');

-- --------------------------------------------------------

--
-- Table structure for table `subject_academic_levels`
--
-- Creation: Oct 03, 2026 at 01:47 PM
--

DROP TABLE IF EXISTS `subject_academic_levels`;
CREATE TABLE `subject_academic_levels` (
  `subject_academic_level_id` int(10) UNSIGNED NOT NULL,
  `subject_id` int(10) UNSIGNED NOT NULL,
  `academic_level_id` int(10) UNSIGNED NOT NULL,
  `status` varchar(30) NOT NULL DEFAULT 'Active'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `subject_academic_levels`
--

INSERT DELAYED IGNORE INTO `subject_academic_levels` (`subject_academic_level_id`, `subject_id`, `academic_level_id`, `status`) VALUES
(1, 1, 1, 'Active'),
(2, 1, 2, 'Active'),
(3, 2, 1, 'Active'),
(4, 2, 2, 'Active'),
(5, 3, 2, 'Active'),
(6, 4, 1, 'Active'),
(7, 4, 2, 'Active'),
(8, 5, 1, 'Active'),
(9, 6, 1, 'Active'),
(10, 6, 2, 'Active'),
(11, 7, 1, 'Active'),
(12, 7, 2, 'Active'),
(13, 8, 1, 'Active'),
(14, 8, 2, 'Active'),
(15, 9, 2, 'Active'),
(16, 14, 1, 'Active'),
(17, 10, 1, 'Active'),
(18, 11, 1, 'Active'),
(19, 12, 1, 'Active'),
(20, 13, 1, 'Active');

-- --------------------------------------------------------

--
-- Table structure for table `subject_combinations`
--
-- Creation: Oct 03, 2026 at 10:52 AM
--

DROP TABLE IF EXISTS `subject_combinations`;
CREATE TABLE `subject_combinations` (
  `combination_id` int(10) UNSIGNED NOT NULL,
  `academic_level_id` int(10) UNSIGNED NOT NULL,
  `combination_code` varchar(50) NOT NULL,
  `combination_name` varchar(100) NOT NULL,
  `description` varchar(255) DEFAULT NULL,
  `status` varchar(30) NOT NULL DEFAULT 'Active'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `subject_combinations`
--

INSERT DELAYED IGNORE INTO `subject_combinations` (`combination_id`, `academic_level_id`, `combination_code`, `combination_name`, `description`, `status`) VALUES
(1, 2, 'BCM', 'Biology, Chemistry and Mathematics', NULL, 'Active');

-- --------------------------------------------------------

--
-- Table structure for table `subject_papers`
--
-- Creation: Oct 03, 2026 at 02:13 PM
--

DROP TABLE IF EXISTS `subject_papers`;
CREATE TABLE `subject_papers` (
  `paper_id` int(10) UNSIGNED NOT NULL,
  `curriculum_subject_id` int(10) UNSIGNED DEFAULT NULL,
  `subject_id` int(10) UNSIGNED DEFAULT NULL,
  `academic_level_id` int(10) UNSIGNED DEFAULT NULL,
  `paper_code` varchar(30) NOT NULL,
  `paper_number` varchar(20) DEFAULT NULL,
  `paper_name` varchar(150) DEFAULT NULL,
  `paper_type` varchar(30) NOT NULL DEFAULT 'Theory',
  `status` varchar(30) NOT NULL DEFAULT 'Active'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `subject_papers`
--

INSERT DELAYED IGNORE INTO `subject_papers` (`paper_id`, `curriculum_subject_id`, `subject_id`, `academic_level_id`, `paper_code`, `paper_number`, `paper_name`, `paper_type`, `status`) VALUES
(1, 1, 1, 2, 'P425', '3', 'Principle Mathematics', 'Theory', 'Active'),
(2, 1, 1, 2, '245', '2', 'Biology', 'Theory', 'Active');

-- --------------------------------------------------------

--
-- Table structure for table `teachers`
--
-- Creation: Oct 02, 2026 at 11:11 AM
--

DROP TABLE IF EXISTS `teachers`;
CREATE TABLE `teachers` (
  `teacher_id` int(10) UNSIGNED NOT NULL,
  `staff_number` varchar(50) NOT NULL,
  `first_name` varchar(100) NOT NULL,
  `middle_name` varchar(100) DEFAULT NULL,
  `last_name` varchar(100) NOT NULL,
  `gender` varchar(30) DEFAULT NULL,
  `phone_number` varchar(30) DEFAULT NULL,
  `email` varchar(150) DEFAULT NULL,
  `address` varchar(255) DEFAULT NULL,
  `employment_status` varchar(30) NOT NULL DEFAULT 'Active',
  `created_at` timestamp NOT NULL DEFAULT current_timestamp(),
  `updated_at` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `teachers`
--

INSERT DELAYED IGNORE INTO `teachers` (`teacher_id`, `staff_number`, `first_name`, `middle_name`, `last_name`, `gender`, `phone_number`, `email`, `address`, `employment_status`, `created_at`, `updated_at`) VALUES
(1, 'TCH/2026/0001', 'Kato', '', 'Francis', 'Male', '0707902789', '', 'Kampala,Uganda', 'Active', '2026-10-03 01:06:28', '2026-10-03 01:06:28'),
(2, 'TCH/2026/0002', 'Kizito', 'John', 'Deo', 'Male', '0785776766', 'johndeo2@gmail.com', 'Kasokwe,Mbale', 'Active', '2026-10-03 02:53:21', '2026-10-03 02:53:21');

-- --------------------------------------------------------

--
-- Table structure for table `teacher_assignments`
--
-- Creation: Oct 02, 2026 at 11:11 AM
--

DROP TABLE IF EXISTS `teacher_assignments`;
CREATE TABLE `teacher_assignments` (
  `teacher_assignment_id` int(10) UNSIGNED NOT NULL,
  `teacher_id` int(10) UNSIGNED NOT NULL,
  `class_subject_id` int(10) UNSIGNED NOT NULL,
  `term_id` int(10) UNSIGNED DEFAULT NULL,
  `status` varchar(30) NOT NULL DEFAULT 'Active'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `teacher_staff_number_sequences`
--
-- Creation: Oct 03, 2026 at 01:02 AM
--

DROP TABLE IF EXISTS `teacher_staff_number_sequences`;
CREATE TABLE `teacher_staff_number_sequences` (
  `staff_year` int(11) NOT NULL,
  `last_sequence` int(11) NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `teacher_staff_number_sequences`
--

INSERT DELAYED IGNORE INTO `teacher_staff_number_sequences` (`staff_year`, `last_sequence`) VALUES
(2026, 2);

-- --------------------------------------------------------

--
-- Table structure for table `terms`
--
-- Creation: Oct 02, 2026 at 11:08 AM
--

DROP TABLE IF EXISTS `terms`;
CREATE TABLE `terms` (
  `term_id` int(10) UNSIGNED NOT NULL,
  `academic_year_id` int(10) UNSIGNED NOT NULL,
  `term_name` varchar(50) NOT NULL,
  `start_date` date DEFAULT NULL,
  `end_date` date DEFAULT NULL,
  `status` enum('Active','Inactive') NOT NULL DEFAULT 'Active'
) ;

--
-- Dumping data for table `terms`
--

INSERT DELAYED IGNORE INTO `terms` (`term_id`, `academic_year_id`, `term_name`, `start_date`, `end_date`, `status`) VALUES
(1, 3, 'Term 1', NULL, NULL, 'Active'),
(2, 3, 'Term 2', NULL, NULL, 'Inactive'),
(3, 3, 'Term 3', NULL, NULL, 'Inactive');

-- --------------------------------------------------------

--
-- Table structure for table `timetable_entries`
--
-- Creation: Oct 02, 2026 at 11:11 AM
--

DROP TABLE IF EXISTS `timetable_entries`;
CREATE TABLE `timetable_entries` (
  `timetable_entry_id` int(10) UNSIGNED NOT NULL,
  `academic_year_id` int(10) UNSIGNED NOT NULL,
  `term_id` int(10) UNSIGNED NOT NULL,
  `class_id` int(10) UNSIGNED NOT NULL,
  `stream_id` int(10) UNSIGNED DEFAULT NULL,
  `subject_id` int(10) UNSIGNED NOT NULL,
  `teacher_id` int(10) UNSIGNED DEFAULT NULL,
  `day_of_week` varchar(30) NOT NULL,
  `start_time` time NOT NULL,
  `end_time` time NOT NULL,
  `room` varchar(100) DEFAULT NULL,
  `status` varchar(30) NOT NULL DEFAULT 'Active',
  `notes` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `users`
--
-- Creation: Oct 03, 2026 at 09:36 PM
--

DROP TABLE IF EXISTS `users`;
CREATE TABLE `users` (
  `user_id` int(10) UNSIGNED NOT NULL,
  `username` varchar(50) NOT NULL,
  `email` varchar(190) DEFAULT NULL,
  `password_hash` varchar(255) NOT NULL,
  `full_name` varchar(150) NOT NULL,
  `role_id` int(10) UNSIGNED NOT NULL,
  `status` enum('Active','Inactive') NOT NULL DEFAULT 'Active',
  `created_at` timestamp NOT NULL DEFAULT current_timestamp(),
  `updated_at` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `must_change_password` tinyint(1) NOT NULL DEFAULT 1,
  `password_changed_at` datetime DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `users`
--

INSERT DELAYED IGNORE INTO `users` (`user_id`, `username`, `email`, `password_hash`, `full_name`, `role_id`, `status`, `created_at`, `updated_at`, `must_change_password`, `password_changed_at`) VALUES
(1, 'admin', NULL, 'PBKDF2-SHA256$100000$Ot361XagzL5wJrGA6oYnig==$J3XSNAzcTwlkJ3huQiAZE+4OQKBCaaxoMKvMyFp4UmU=', 'System Administrator', 1, 'Active', '2026-10-03 02:24:33', '2026-10-03 17:11:47', 0, '2026-10-03 20:11:47');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `academic_levels`
--
ALTER TABLE `academic_levels`
  ADD PRIMARY KEY (`academic_level_id`),
  ADD UNIQUE KEY `uq_academic_levels_code` (`level_code`),
  ADD UNIQUE KEY `uq_academic_levels_name` (`level_name`);

--
-- Indexes for table `academic_years`
--
ALTER TABLE `academic_years`
  ADD PRIMARY KEY (`academic_year_id`),
  ADD UNIQUE KEY `year_name` (`year_name`),
  ADD KEY `fk_academic_years_curriculum` (`curriculum_id`);

--
-- Indexes for table `attendance_records`
--
ALTER TABLE `attendance_records`
  ADD PRIMARY KEY (`attendance_record_id`),
  ADD UNIQUE KEY `attendance_session_id` (`attendance_session_id`,`student_id`),
  ADD UNIQUE KEY `uq_attendance_session_student` (`attendance_session_id`,`student_id`),
  ADD KEY `fk_attendance_records_student` (`student_id`);

--
-- Indexes for table `attendance_sessions`
--
ALTER TABLE `attendance_sessions`
  ADD PRIMARY KEY (`attendance_session_id`),
  ADD UNIQUE KEY `attendance_date` (`attendance_date`,`academic_year_id`,`term_id`,`class_id`,`stream_id`),
  ADD KEY `fk_attendance_sessions_year` (`academic_year_id`),
  ADD KEY `fk_attendance_sessions_term` (`term_id`),
  ADD KEY `fk_attendance_sessions_class` (`class_id`),
  ADD KEY `fk_attendance_sessions_stream` (`stream_id`),
  ADD KEY `fk_attendance_sessions_user` (`recorded_by`);

--
-- Indexes for table `audit_logs`
--
ALTER TABLE `audit_logs`
  ADD PRIMARY KEY (`audit_log_id`),
  ADD KEY `fk_audit_logs_user` (`user_id`);

--
-- Indexes for table `classes`
--
ALTER TABLE `classes`
  ADD PRIMARY KEY (`class_id`),
  ADD UNIQUE KEY `class_name` (`class_name`),
  ADD UNIQUE KEY `uq_class_name` (`class_name`),
  ADD KEY `fk_classes_academic_level` (`academic_level_id`);

--
-- Indexes for table `class_subjects`
--
ALTER TABLE `class_subjects`
  ADD PRIMARY KEY (`class_subject_id`),
  ADD KEY `fk_class_subjects_year` (`academic_year_id`),
  ADD KEY `fk_class_subjects_class` (`class_id`),
  ADD KEY `fk_class_subjects_stream` (`stream_id`),
  ADD KEY `fk_class_subjects_subject` (`subject_id`);

--
-- Indexes for table `combination_subjects`
--
ALTER TABLE `combination_subjects`
  ADD PRIMARY KEY (`combination_subject_id`),
  ADD UNIQUE KEY `uq_combination_subject` (`combination_id`,`subject_id`),
  ADD KEY `fk_combination_subjects_subject` (`subject_id`);

--
-- Indexes for table `curricula`
--
ALTER TABLE `curricula`
  ADD PRIMARY KEY (`curriculum_id`),
  ADD UNIQUE KEY `uq_curricula_code` (`curriculum_code`),
  ADD UNIQUE KEY `uq_curricula_name` (`curriculum_name`),
  ADD KEY `fk_curricula_academic_level` (`academic_level_id`);

--
-- Indexes for table `curriculum_subjects`
--
ALTER TABLE `curriculum_subjects`
  ADD PRIMARY KEY (`curriculum_subject_id`),
  ADD UNIQUE KEY `uq_curriculum_subject` (`curriculum_id`,`subject_id`),
  ADD UNIQUE KEY `uq_curriculum_subject_code` (`curriculum_id`,`uneb_subject_code`),
  ADD KEY `fk_curriculum_subjects_subject` (`subject_id`);

--
-- Indexes for table `discipline_records`
--
ALTER TABLE `discipline_records`
  ADD PRIMARY KEY (`discipline_record_id`),
  ADD KEY `fk_discipline_student` (`student_id`),
  ADD KEY `fk_discipline_user` (`recorded_by`);

--
-- Indexes for table `enrollments`
--
ALTER TABLE `enrollments`
  ADD PRIMARY KEY (`enrollment_id`),
  ADD UNIQUE KEY `student_id` (`student_id`,`academic_year_id`,`term_id`),
  ADD KEY `fk_enrollments_year` (`academic_year_id`),
  ADD KEY `fk_enrollments_term` (`term_id`),
  ADD KEY `fk_enrollments_class` (`class_id`),
  ADD KEY `fk_enrollments_stream` (`stream_id`),
  ADD KEY `fk_enrollments_combination` (`combination_id`);

--
-- Indexes for table `enrollment_optional_subjects`
--
ALTER TABLE `enrollment_optional_subjects`
  ADD PRIMARY KEY (`enrollment_optional_subject_id`),
  ADD UNIQUE KEY `uq_enrollment_optional_subject` (`enrollment_id`,`subject_id`),
  ADD UNIQUE KEY `uq_enrollment_optional_number` (`enrollment_id`,`option_number`),
  ADD KEY `fk_enrollment_optional_subjects_subject` (`subject_id`);

--
-- Indexes for table `examinations`
--
ALTER TABLE `examinations`
  ADD PRIMARY KEY (`examination_id`),
  ADD KEY `fk_examinations_year` (`academic_year_id`),
  ADD KEY `fk_examinations_term` (`term_id`),
  ADD KEY `fk_examinations_class` (`class_id`),
  ADD KEY `fk_examinations_approved_by` (`approved_by`);

--
-- Indexes for table `examination_papers`
--
ALTER TABLE `examination_papers`
  ADD PRIMARY KEY (`examination_paper_id`),
  ADD UNIQUE KEY `uq_examination_subject_paper` (`examination_subject_id`,`paper_id`),
  ADD KEY `fk_examination_papers_paper` (`paper_id`);

--
-- Indexes for table `examination_subjects`
--
ALTER TABLE `examination_subjects`
  ADD PRIMARY KEY (`examination_subject_id`),
  ADD UNIQUE KEY `examination_id` (`examination_id`,`subject_id`),
  ADD UNIQUE KEY `uq_examination_subject` (`examination_id`,`subject_id`),
  ADD KEY `fk_examination_subjects_subject` (`subject_id`);

--
-- Indexes for table `fee_charges`
--
ALTER TABLE `fee_charges`
  ADD PRIMARY KEY (`fee_charge_id`),
  ADD UNIQUE KEY `student_id` (`student_id`,`fee_structure_id`),
  ADD KEY `fk_fee_charges_structure` (`fee_structure_id`);

--
-- Indexes for table `fee_structures`
--
ALTER TABLE `fee_structures`
  ADD PRIMARY KEY (`fee_structure_id`),
  ADD KEY `fk_fee_structures_year` (`academic_year_id`),
  ADD KEY `fk_fee_structures_term` (`term_id`),
  ADD KEY `fk_fee_structures_class` (`class_id`),
  ADD KEY `fk_fee_structures_stream` (`stream_id`);

--
-- Indexes for table `grading_scales`
--
ALTER TABLE `grading_scales`
  ADD PRIMARY KEY (`grading_scale_id`),
  ADD UNIQUE KEY `uq_grading_scale_level_name_grade` (`academic_level_id`,`scale_name`,`grade`);

--
-- Indexes for table `guardians`
--
ALTER TABLE `guardians`
  ADD PRIMARY KEY (`guardian_id`);

--
-- Indexes for table `marks`
--
ALTER TABLE `marks`
  ADD PRIMARY KEY (`mark_id`),
  ADD UNIQUE KEY `uq_marks_examination_paper_student` (`examination_paper_id`,`student_id`),
  ADD KEY `fk_marks_student` (`student_id`),
  ADD KEY `fk_marks_entered_by` (`entered_by`),
  ADD KEY `idx_marks_examination_subject` (`examination_subject_id`),
  ADD KEY `idx_marks_examination_paper_student` (`examination_paper_id`,`student_id`);

--
-- Indexes for table `notices`
--
ALTER TABLE `notices`
  ADD PRIMARY KEY (`notice_id`),
  ADD KEY `fk_notices_user` (`created_by`);

--
-- Indexes for table `payments`
--
ALTER TABLE `payments`
  ADD PRIMARY KEY (`payment_id`),
  ADD UNIQUE KEY `receipt_number` (`receipt_number`),
  ADD KEY `fk_payments_student` (`student_id`),
  ADD KEY `fk_payments_user` (`received_by`);

--
-- Indexes for table `payment_allocations`
--
ALTER TABLE `payment_allocations`
  ADD PRIMARY KEY (`payment_allocation_id`),
  ADD KEY `fk_payment_allocations_payment` (`payment_id`),
  ADD KEY `fk_payment_allocations_charge` (`fee_charge_id`);

--
-- Indexes for table `permissions`
--
ALTER TABLE `permissions`
  ADD PRIMARY KEY (`permission_id`),
  ADD UNIQUE KEY `permission_name` (`permission_name`);

--
-- Indexes for table `roles`
--
ALTER TABLE `roles`
  ADD PRIMARY KEY (`role_id`),
  ADD UNIQUE KEY `role_name` (`role_name`);

--
-- Indexes for table `role_permissions`
--
ALTER TABLE `role_permissions`
  ADD PRIMARY KEY (`role_id`,`permission_id`),
  ADD KEY `fk_role_permissions_permission` (`permission_id`);

--
-- Indexes for table `school_settings`
--
ALTER TABLE `school_settings`
  ADD PRIMARY KEY (`setting_id`),
  ADD UNIQUE KEY `setting_key` (`setting_key`);

--
-- Indexes for table `streams`
--
ALTER TABLE `streams`
  ADD PRIMARY KEY (`stream_id`),
  ADD UNIQUE KEY `class_id` (`class_id`,`stream_name`);

--
-- Indexes for table `students`
--
ALTER TABLE `students`
  ADD PRIMARY KEY (`student_id`),
  ADD UNIQUE KEY `registration_number` (`registration_number`),
  ADD UNIQUE KEY `uq_student_year_sequence` (`registration_year`,`registration_sequence`);

--
-- Indexes for table `student_guardians`
--
ALTER TABLE `student_guardians`
  ADD PRIMARY KEY (`student_id`,`guardian_id`),
  ADD KEY `fk_student_guardians_guardian` (`guardian_id`);

--
-- Indexes for table `student_registration_sequences`
--
ALTER TABLE `student_registration_sequences`
  ADD PRIMARY KEY (`registration_year`);

--
-- Indexes for table `student_subject_results`
--
ALTER TABLE `student_subject_results`
  ADD PRIMARY KEY (`student_subject_result_id`),
  ADD UNIQUE KEY `uq_student_subject_result` (`enrollment_id`,`examination_subject_id`),
  ADD KEY `idx_student_subject_results_enrollment` (`enrollment_id`),
  ADD KEY `idx_student_subject_results_examination_subject` (`examination_subject_id`);

--
-- Indexes for table `subjects`
--
ALTER TABLE `subjects`
  ADD PRIMARY KEY (`subject_id`),
  ADD UNIQUE KEY `subject_code` (`subject_code`),
  ADD UNIQUE KEY `subject_name` (`subject_name`);

--
-- Indexes for table `subject_academic_levels`
--
ALTER TABLE `subject_academic_levels`
  ADD PRIMARY KEY (`subject_academic_level_id`),
  ADD UNIQUE KEY `uq_subject_academic_level` (`subject_id`,`academic_level_id`),
  ADD KEY `fk_subject_academic_levels_level` (`academic_level_id`);

--
-- Indexes for table `subject_combinations`
--
ALTER TABLE `subject_combinations`
  ADD PRIMARY KEY (`combination_id`),
  ADD UNIQUE KEY `uq_subject_combinations_code` (`combination_code`),
  ADD KEY `fk_subject_combinations_level` (`academic_level_id`);

--
-- Indexes for table `subject_papers`
--
ALTER TABLE `subject_papers`
  ADD PRIMARY KEY (`paper_id`),
  ADD UNIQUE KEY `uq_curriculum_subject_paper_code` (`curriculum_subject_id`,`paper_code`),
  ADD KEY `fk_subject_papers_level` (`academic_level_id`),
  ADD KEY `idx_subject_papers_subject_id` (`subject_id`);

--
-- Indexes for table `teachers`
--
ALTER TABLE `teachers`
  ADD PRIMARY KEY (`teacher_id`),
  ADD UNIQUE KEY `staff_number` (`staff_number`);

--
-- Indexes for table `teacher_assignments`
--
ALTER TABLE `teacher_assignments`
  ADD PRIMARY KEY (`teacher_assignment_id`),
  ADD KEY `fk_teacher_assignments_teacher` (`teacher_id`),
  ADD KEY `fk_teacher_assignments_class_subject` (`class_subject_id`),
  ADD KEY `fk_teacher_assignments_term` (`term_id`);

--
-- Indexes for table `teacher_staff_number_sequences`
--
ALTER TABLE `teacher_staff_number_sequences`
  ADD PRIMARY KEY (`staff_year`);

--
-- Indexes for table `terms`
--
ALTER TABLE `terms`
  ADD PRIMARY KEY (`term_id`),
  ADD UNIQUE KEY `academic_year_id` (`academic_year_id`,`term_name`),
  ADD UNIQUE KEY `uq_term_per_year` (`academic_year_id`,`term_name`);

--
-- Indexes for table `timetable_entries`
--
ALTER TABLE `timetable_entries`
  ADD PRIMARY KEY (`timetable_entry_id`),
  ADD KEY `fk_timetable_year` (`academic_year_id`),
  ADD KEY `fk_timetable_term` (`term_id`),
  ADD KEY `fk_timetable_class` (`class_id`),
  ADD KEY `fk_timetable_stream` (`stream_id`),
  ADD KEY `fk_timetable_subject` (`subject_id`),
  ADD KEY `fk_timetable_teacher` (`teacher_id`);

--
-- Indexes for table `users`
--
ALTER TABLE `users`
  ADD PRIMARY KEY (`user_id`),
  ADD UNIQUE KEY `username` (`username`),
  ADD KEY `fk_users_role` (`role_id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `academic_levels`
--
ALTER TABLE `academic_levels`
  MODIFY `academic_level_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=3;

--
-- AUTO_INCREMENT for table `academic_years`
--
ALTER TABLE `academic_years`
  MODIFY `academic_year_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `attendance_records`
--
ALTER TABLE `attendance_records`
  MODIFY `attendance_record_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `attendance_sessions`
--
ALTER TABLE `attendance_sessions`
  MODIFY `attendance_session_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `audit_logs`
--
ALTER TABLE `audit_logs`
  MODIFY `audit_log_id` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `classes`
--
ALTER TABLE `classes`
  MODIFY `class_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `class_subjects`
--
ALTER TABLE `class_subjects`
  MODIFY `class_subject_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `combination_subjects`
--
ALTER TABLE `combination_subjects`
  MODIFY `combination_subject_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- AUTO_INCREMENT for table `curricula`
--
ALTER TABLE `curricula`
  MODIFY `curriculum_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=3;

--
-- AUTO_INCREMENT for table `curriculum_subjects`
--
ALTER TABLE `curriculum_subjects`
  MODIFY `curriculum_subject_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT for table `discipline_records`
--
ALTER TABLE `discipline_records`
  MODIFY `discipline_record_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `enrollments`
--
ALTER TABLE `enrollments`
  MODIFY `enrollment_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT for table `enrollment_optional_subjects`
--
ALTER TABLE `enrollment_optional_subjects`
  MODIFY `enrollment_optional_subject_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `examinations`
--
ALTER TABLE `examinations`
  MODIFY `examination_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;

--
-- AUTO_INCREMENT for table `examination_papers`
--
ALTER TABLE `examination_papers`
  MODIFY `examination_paper_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `examination_subjects`
--
ALTER TABLE `examination_subjects`
  MODIFY `examination_subject_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `fee_charges`
--
ALTER TABLE `fee_charges`
  MODIFY `fee_charge_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `fee_structures`
--
ALTER TABLE `fee_structures`
  MODIFY `fee_structure_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `grading_scales`
--
ALTER TABLE `grading_scales`
  MODIFY `grading_scale_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=14;

--
-- AUTO_INCREMENT for table `guardians`
--
ALTER TABLE `guardians`
  MODIFY `guardian_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT for table `marks`
--
ALTER TABLE `marks`
  MODIFY `mark_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `notices`
--
ALTER TABLE `notices`
  MODIFY `notice_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `payments`
--
ALTER TABLE `payments`
  MODIFY `payment_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `payment_allocations`
--
ALTER TABLE `payment_allocations`
  MODIFY `payment_allocation_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `permissions`
--
ALTER TABLE `permissions`
  MODIFY `permission_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=29;

--
-- AUTO_INCREMENT for table `roles`
--
ALTER TABLE `roles`
  MODIFY `role_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- AUTO_INCREMENT for table `school_settings`
--
ALTER TABLE `school_settings`
  MODIFY `setting_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `streams`
--
ALTER TABLE `streams`
  MODIFY `stream_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=11;

--
-- AUTO_INCREMENT for table `students`
--
ALTER TABLE `students`
  MODIFY `student_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT for table `student_subject_results`
--
ALTER TABLE `student_subject_results`
  MODIFY `student_subject_result_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `subjects`
--
ALTER TABLE `subjects`
  MODIFY `subject_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=15;

--
-- AUTO_INCREMENT for table `subject_academic_levels`
--
ALTER TABLE `subject_academic_levels`
  MODIFY `subject_academic_level_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=21;

--
-- AUTO_INCREMENT for table `subject_combinations`
--
ALTER TABLE `subject_combinations`
  MODIFY `combination_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;

--
-- AUTO_INCREMENT for table `subject_papers`
--
ALTER TABLE `subject_papers`
  MODIFY `paper_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=3;

--
-- AUTO_INCREMENT for table `teachers`
--
ALTER TABLE `teachers`
  MODIFY `teacher_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=3;

--
-- AUTO_INCREMENT for table `teacher_assignments`
--
ALTER TABLE `teacher_assignments`
  MODIFY `teacher_assignment_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `terms`
--
ALTER TABLE `terms`
  MODIFY `term_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `timetable_entries`
--
ALTER TABLE `timetable_entries`
  MODIFY `timetable_entry_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `users`
--
ALTER TABLE `users`
  MODIFY `user_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `academic_years`
--
ALTER TABLE `academic_years`
  ADD CONSTRAINT `fk_academic_years_curriculum` FOREIGN KEY (`curriculum_id`) REFERENCES `curricula` (`curriculum_id`) ON UPDATE CASCADE;

--
-- Constraints for table `attendance_records`
--
ALTER TABLE `attendance_records`
  ADD CONSTRAINT `fk_attendance_records_session` FOREIGN KEY (`attendance_session_id`) REFERENCES `attendance_sessions` (`attendance_session_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_attendance_records_student` FOREIGN KEY (`student_id`) REFERENCES `students` (`student_id`) ON UPDATE CASCADE;

--
-- Constraints for table `attendance_sessions`
--
ALTER TABLE `attendance_sessions`
  ADD CONSTRAINT `fk_attendance_sessions_class` FOREIGN KEY (`class_id`) REFERENCES `classes` (`class_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_attendance_sessions_stream` FOREIGN KEY (`stream_id`) REFERENCES `streams` (`stream_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_attendance_sessions_term` FOREIGN KEY (`term_id`) REFERENCES `terms` (`term_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_attendance_sessions_user` FOREIGN KEY (`recorded_by`) REFERENCES `users` (`user_id`) ON DELETE SET NULL ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_attendance_sessions_year` FOREIGN KEY (`academic_year_id`) REFERENCES `academic_years` (`academic_year_id`) ON UPDATE CASCADE;

--
-- Constraints for table `audit_logs`
--
ALTER TABLE `audit_logs`
  ADD CONSTRAINT `fk_audit_logs_user` FOREIGN KEY (`user_id`) REFERENCES `users` (`user_id`) ON DELETE SET NULL ON UPDATE CASCADE;

--
-- Constraints for table `classes`
--
ALTER TABLE `classes`
  ADD CONSTRAINT `fk_classes_academic_level` FOREIGN KEY (`academic_level_id`) REFERENCES `academic_levels` (`academic_level_id`) ON UPDATE CASCADE;

--
-- Constraints for table `class_subjects`
--
ALTER TABLE `class_subjects`
  ADD CONSTRAINT `fk_class_subjects_class` FOREIGN KEY (`class_id`) REFERENCES `classes` (`class_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_class_subjects_stream` FOREIGN KEY (`stream_id`) REFERENCES `streams` (`stream_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_class_subjects_subject` FOREIGN KEY (`subject_id`) REFERENCES `subjects` (`subject_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_class_subjects_year` FOREIGN KEY (`academic_year_id`) REFERENCES `academic_years` (`academic_year_id`) ON UPDATE CASCADE;

--
-- Constraints for table `combination_subjects`
--
ALTER TABLE `combination_subjects`
  ADD CONSTRAINT `fk_combination_subjects_combination` FOREIGN KEY (`combination_id`) REFERENCES `subject_combinations` (`combination_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_combination_subjects_subject` FOREIGN KEY (`subject_id`) REFERENCES `subjects` (`subject_id`) ON UPDATE CASCADE;

--
-- Constraints for table `curricula`
--
ALTER TABLE `curricula`
  ADD CONSTRAINT `fk_curricula_academic_level` FOREIGN KEY (`academic_level_id`) REFERENCES `academic_levels` (`academic_level_id`) ON UPDATE CASCADE;

--
-- Constraints for table `curriculum_subjects`
--
ALTER TABLE `curriculum_subjects`
  ADD CONSTRAINT `fk_curriculum_subjects_curriculum` FOREIGN KEY (`curriculum_id`) REFERENCES `curricula` (`curriculum_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_curriculum_subjects_subject` FOREIGN KEY (`subject_id`) REFERENCES `subjects` (`subject_id`) ON UPDATE CASCADE;

--
-- Constraints for table `discipline_records`
--
ALTER TABLE `discipline_records`
  ADD CONSTRAINT `fk_discipline_student` FOREIGN KEY (`student_id`) REFERENCES `students` (`student_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_discipline_user` FOREIGN KEY (`recorded_by`) REFERENCES `users` (`user_id`) ON DELETE SET NULL ON UPDATE CASCADE;

--
-- Constraints for table `enrollments`
--
ALTER TABLE `enrollments`
  ADD CONSTRAINT `fk_enrollments_class` FOREIGN KEY (`class_id`) REFERENCES `classes` (`class_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_enrollments_combination` FOREIGN KEY (`combination_id`) REFERENCES `subject_combinations` (`combination_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_enrollments_stream` FOREIGN KEY (`stream_id`) REFERENCES `streams` (`stream_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_enrollments_student` FOREIGN KEY (`student_id`) REFERENCES `students` (`student_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_enrollments_term` FOREIGN KEY (`term_id`) REFERENCES `terms` (`term_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_enrollments_year` FOREIGN KEY (`academic_year_id`) REFERENCES `academic_years` (`academic_year_id`) ON UPDATE CASCADE;

--
-- Constraints for table `enrollment_optional_subjects`
--
ALTER TABLE `enrollment_optional_subjects`
  ADD CONSTRAINT `fk_enrollment_optional_subjects_enrollment` FOREIGN KEY (`enrollment_id`) REFERENCES `enrollments` (`enrollment_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_enrollment_optional_subjects_subject` FOREIGN KEY (`subject_id`) REFERENCES `subjects` (`subject_id`) ON UPDATE CASCADE;

--
-- Constraints for table `examinations`
--
ALTER TABLE `examinations`
  ADD CONSTRAINT `fk_examinations_approved_by` FOREIGN KEY (`approved_by`) REFERENCES `users` (`user_id`) ON DELETE SET NULL ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_examinations_class` FOREIGN KEY (`class_id`) REFERENCES `classes` (`class_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_examinations_term` FOREIGN KEY (`term_id`) REFERENCES `terms` (`term_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_examinations_year` FOREIGN KEY (`academic_year_id`) REFERENCES `academic_years` (`academic_year_id`) ON UPDATE CASCADE;

--
-- Constraints for table `examination_papers`
--
ALTER TABLE `examination_papers`
  ADD CONSTRAINT `fk_examination_papers_paper` FOREIGN KEY (`paper_id`) REFERENCES `subject_papers` (`paper_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_examination_papers_subject` FOREIGN KEY (`examination_subject_id`) REFERENCES `examination_subjects` (`examination_subject_id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `examination_subjects`
--
ALTER TABLE `examination_subjects`
  ADD CONSTRAINT `fk_examination_subjects_exam` FOREIGN KEY (`examination_id`) REFERENCES `examinations` (`examination_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_examination_subjects_subject` FOREIGN KEY (`subject_id`) REFERENCES `subjects` (`subject_id`) ON UPDATE CASCADE;

--
-- Constraints for table `fee_charges`
--
ALTER TABLE `fee_charges`
  ADD CONSTRAINT `fk_fee_charges_structure` FOREIGN KEY (`fee_structure_id`) REFERENCES `fee_structures` (`fee_structure_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_fee_charges_student` FOREIGN KEY (`student_id`) REFERENCES `students` (`student_id`) ON UPDATE CASCADE;

--
-- Constraints for table `fee_structures`
--
ALTER TABLE `fee_structures`
  ADD CONSTRAINT `fk_fee_structures_class` FOREIGN KEY (`class_id`) REFERENCES `classes` (`class_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_fee_structures_stream` FOREIGN KEY (`stream_id`) REFERENCES `streams` (`stream_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_fee_structures_term` FOREIGN KEY (`term_id`) REFERENCES `terms` (`term_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_fee_structures_year` FOREIGN KEY (`academic_year_id`) REFERENCES `academic_years` (`academic_year_id`) ON UPDATE CASCADE;

--
-- Constraints for table `grading_scales`
--
ALTER TABLE `grading_scales`
  ADD CONSTRAINT `fk_grading_scales_level` FOREIGN KEY (`academic_level_id`) REFERENCES `academic_levels` (`academic_level_id`) ON UPDATE CASCADE;

--
-- Constraints for table `marks`
--
ALTER TABLE `marks`
  ADD CONSTRAINT `fk_marks_entered_by` FOREIGN KEY (`entered_by`) REFERENCES `users` (`user_id`) ON DELETE SET NULL ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_marks_exam_subject` FOREIGN KEY (`examination_subject_id`) REFERENCES `examination_subjects` (`examination_subject_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_marks_examination_paper` FOREIGN KEY (`examination_paper_id`) REFERENCES `examination_papers` (`examination_paper_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_marks_student` FOREIGN KEY (`student_id`) REFERENCES `students` (`student_id`) ON UPDATE CASCADE;

--
-- Constraints for table `notices`
--
ALTER TABLE `notices`
  ADD CONSTRAINT `fk_notices_user` FOREIGN KEY (`created_by`) REFERENCES `users` (`user_id`) ON DELETE SET NULL ON UPDATE CASCADE;

--
-- Constraints for table `payments`
--
ALTER TABLE `payments`
  ADD CONSTRAINT `fk_payments_student` FOREIGN KEY (`student_id`) REFERENCES `students` (`student_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_payments_user` FOREIGN KEY (`received_by`) REFERENCES `users` (`user_id`) ON DELETE SET NULL ON UPDATE CASCADE;

--
-- Constraints for table `payment_allocations`
--
ALTER TABLE `payment_allocations`
  ADD CONSTRAINT `fk_payment_allocations_charge` FOREIGN KEY (`fee_charge_id`) REFERENCES `fee_charges` (`fee_charge_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_payment_allocations_payment` FOREIGN KEY (`payment_id`) REFERENCES `payments` (`payment_id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `role_permissions`
--
ALTER TABLE `role_permissions`
  ADD CONSTRAINT `fk_role_permissions_permission` FOREIGN KEY (`permission_id`) REFERENCES `permissions` (`permission_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_role_permissions_role` FOREIGN KEY (`role_id`) REFERENCES `roles` (`role_id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `streams`
--
ALTER TABLE `streams`
  ADD CONSTRAINT `fk_streams_class` FOREIGN KEY (`class_id`) REFERENCES `classes` (`class_id`) ON UPDATE CASCADE;

--
-- Constraints for table `student_guardians`
--
ALTER TABLE `student_guardians`
  ADD CONSTRAINT `fk_student_guardians_guardian` FOREIGN KEY (`guardian_id`) REFERENCES `guardians` (`guardian_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_student_guardians_student` FOREIGN KEY (`student_id`) REFERENCES `students` (`student_id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `student_subject_results`
--
ALTER TABLE `student_subject_results`
  ADD CONSTRAINT `fk_ssr_enrollment` FOREIGN KEY (`enrollment_id`) REFERENCES `enrollments` (`enrollment_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_ssr_examination_subject` FOREIGN KEY (`examination_subject_id`) REFERENCES `examination_subjects` (`examination_subject_id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `subject_academic_levels`
--
ALTER TABLE `subject_academic_levels`
  ADD CONSTRAINT `fk_subject_academic_levels_level` FOREIGN KEY (`academic_level_id`) REFERENCES `academic_levels` (`academic_level_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_subject_academic_levels_subject` FOREIGN KEY (`subject_id`) REFERENCES `subjects` (`subject_id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `subject_combinations`
--
ALTER TABLE `subject_combinations`
  ADD CONSTRAINT `fk_subject_combinations_level` FOREIGN KEY (`academic_level_id`) REFERENCES `academic_levels` (`academic_level_id`) ON UPDATE CASCADE;

--
-- Constraints for table `subject_papers`
--
ALTER TABLE `subject_papers`
  ADD CONSTRAINT `fk_subject_papers_curriculum_subject` FOREIGN KEY (`curriculum_subject_id`) REFERENCES `curriculum_subjects` (`curriculum_subject_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_subject_papers_level` FOREIGN KEY (`academic_level_id`) REFERENCES `academic_levels` (`academic_level_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_subject_papers_subject` FOREIGN KEY (`subject_id`) REFERENCES `subjects` (`subject_id`) ON UPDATE CASCADE;

--
-- Constraints for table `teacher_assignments`
--
ALTER TABLE `teacher_assignments`
  ADD CONSTRAINT `fk_teacher_assignments_class_subject` FOREIGN KEY (`class_subject_id`) REFERENCES `class_subjects` (`class_subject_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_teacher_assignments_teacher` FOREIGN KEY (`teacher_id`) REFERENCES `teachers` (`teacher_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_teacher_assignments_term` FOREIGN KEY (`term_id`) REFERENCES `terms` (`term_id`) ON UPDATE CASCADE;

--
-- Constraints for table `terms`
--
ALTER TABLE `terms`
  ADD CONSTRAINT `fk_terms_academic_year` FOREIGN KEY (`academic_year_id`) REFERENCES `academic_years` (`academic_year_id`) ON UPDATE CASCADE;

--
-- Constraints for table `timetable_entries`
--
ALTER TABLE `timetable_entries`
  ADD CONSTRAINT `fk_timetable_class` FOREIGN KEY (`class_id`) REFERENCES `classes` (`class_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_timetable_stream` FOREIGN KEY (`stream_id`) REFERENCES `streams` (`stream_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_timetable_subject` FOREIGN KEY (`subject_id`) REFERENCES `subjects` (`subject_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_timetable_teacher` FOREIGN KEY (`teacher_id`) REFERENCES `teachers` (`teacher_id`) ON DELETE SET NULL ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_timetable_term` FOREIGN KEY (`term_id`) REFERENCES `terms` (`term_id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_timetable_year` FOREIGN KEY (`academic_year_id`) REFERENCES `academic_years` (`academic_year_id`) ON UPDATE CASCADE;

--
-- Constraints for table `users`
--
ALTER TABLE `users`
  ADD CONSTRAINT `fk_users_role` FOREIGN KEY (`role_id`) REFERENCES `roles` (`role_id`) ON UPDATE CASCADE;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
