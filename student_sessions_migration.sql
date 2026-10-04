-- ============================================================
-- SchoolCore Student Sessions
--
-- Session is part of an enrollment, not a permanent student
-- attribute. This supports Day <-> Boarding changes while
-- preserving historical academic and financial records.
--
-- Standard sessions:
--   1. Day
--   2. Boarding
-- ============================================================

CREATE TABLE IF NOT EXISTS student_sessions (
    session_id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    session_code VARCHAR(30) NOT NULL,
    session_name VARCHAR(80) NOT NULL,
    description VARCHAR(255) DEFAULT NULL,
    status VARCHAR(20) NOT NULL DEFAULT 'Active',
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
        ON UPDATE CURRENT_TIMESTAMP,
    PRIMARY KEY (session_id),
    UNIQUE KEY uq_student_sessions_code (session_code),
    UNIQUE KEY uq_student_sessions_name (session_name),
    KEY idx_student_sessions_status (status)
) ENGINE=InnoDB
  DEFAULT CHARSET=utf8mb4
  COLLATE=utf8mb4_unicode_ci;

INSERT INTO student_sessions
    (session_code, session_name, description, status)
VALUES
    (
        'DAY',
        'Day',
        'Student attends as a day scholar.',
        'Active'
    ),
    (
        'BOARDING',
        'Boarding',
        'Student attends as a boarding student.',
        'Active'
    )
ON DUPLICATE KEY UPDATE
    description = VALUES(description),
    status = VALUES(status);

ALTER TABLE enrollments
    ADD COLUMN IF NOT EXISTS session_id INT(10) UNSIGNED NULL
        AFTER stream_id,
    ADD KEY IF NOT EXISTS idx_enrollments_session (session_id);

SET @fk_session_exists = (
    SELECT COUNT(*)
    FROM information_schema.KEY_COLUMN_USAGE
    WHERE TABLE_SCHEMA = DATABASE()
      AND TABLE_NAME = 'enrollments'
      AND CONSTRAINT_NAME = 'fk_enrollments_session'
);

SET @sql_session_fk = IF(
    @fk_session_exists = 0,
    'ALTER TABLE enrollments ADD CONSTRAINT fk_enrollments_session FOREIGN KEY (session_id) REFERENCES student_sessions(session_id) ON UPDATE CASCADE',
    'SELECT 1'
);

PREPARE stmt_session_fk FROM @sql_session_fk;
EXECUTE stmt_session_fk;
DEALLOCATE PREPARE stmt_session_fk;

-- Existing enrollment history is intentionally not assigned
-- automatically to Day or Boarding. The registration/enrollment
-- workflow will capture the correct session for each placement.
