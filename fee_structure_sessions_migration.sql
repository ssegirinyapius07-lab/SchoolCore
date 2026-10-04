-- ============================================================
-- SchoolCore Fee Structures: Class + Session
--
-- Fee structures are based on:
--   Academic Year + Term + Class + Session
--
-- Stream remains an academic/enrollment concept, but it is not
-- used to determine the standard fee structure.
--
-- Existing stream-based fee structures are preserved for history.
-- Because they have no session_id, they are marked Inactive so
-- they cannot generate new student charges.
-- ============================================================

ALTER TABLE fee_structures
    ADD COLUMN IF NOT EXISTS session_id INT(10) UNSIGNED NULL
        AFTER class_id,
    ADD KEY IF NOT EXISTS idx_fee_structures_session (session_id);

SET @fk_fee_session_exists = (
    SELECT COUNT(*)
    FROM information_schema.KEY_COLUMN_USAGE
    WHERE TABLE_SCHEMA = DATABASE()
      AND TABLE_NAME = 'fee_structures'
      AND CONSTRAINT_NAME = 'fk_fee_structures_session'
);

SET @sql_fee_session_fk = IF(
    @fk_fee_session_exists = 0,
    'ALTER TABLE fee_structures ADD CONSTRAINT fk_fee_structures_session FOREIGN KEY (session_id) REFERENCES student_sessions(session_id) ON UPDATE CASCADE',
    'SELECT 1'
);

PREPARE stmt_fee_session_fk FROM @sql_fee_session_fk;
EXECUTE stmt_fee_session_fk;
DEALLOCATE PREPARE stmt_fee_session_fk;

-- Legacy fee structures without a session must not generate new
-- charges under the new Class + Session model.
UPDATE fee_structures
SET status = 'Inactive'
WHERE session_id IS NULL
  AND status = 'Active';

-- stream_id is intentionally retained for historical compatibility.
-- Existing fee_charges linked to older fee structures remain intact.
