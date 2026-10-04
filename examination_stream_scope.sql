-- SchoolCore examination stream scope
-- All Streams is represented by NULL in examinations.stream_id.
-- A selected stream scopes the examination to that stream.

SET @has_stream_id := (
    SELECT COUNT(*)
    FROM information_schema.COLUMNS
    WHERE TABLE_SCHEMA = DATABASE()
      AND TABLE_NAME = 'examinations'
      AND COLUMN_NAME = 'stream_id'
);

SET @sql := IF(
    @has_stream_id = 0,
    'ALTER TABLE examinations ADD COLUMN stream_id INT(10) UNSIGNED NULL AFTER class_id',
    'SELECT 1'
);

PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

SET @has_stream_index := (
    SELECT COUNT(*)
    FROM information_schema.STATISTICS
    WHERE TABLE_SCHEMA = DATABASE()
      AND TABLE_NAME = 'examinations'
      AND INDEX_NAME = 'idx_examinations_scope'
);

SET @sql := IF(
    @has_stream_index = 0,
    'ALTER TABLE examinations ADD KEY idx_examinations_scope (academic_year_id, term_id, class_id, stream_id, examination_type)',
    'SELECT 1'
);

PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

SET @has_stream_fk := (
    SELECT COUNT(*)
    FROM information_schema.KEY_COLUMN_USAGE
    WHERE TABLE_SCHEMA = DATABASE()
      AND TABLE_NAME = 'examinations'
      AND CONSTRAINT_NAME = 'fk_examinations_stream'
);

SET @sql := IF(
    @has_stream_fk = 0,
    'ALTER TABLE examinations ADD CONSTRAINT fk_examinations_stream FOREIGN KEY (stream_id) REFERENCES streams(stream_id) ON UPDATE CASCADE ON DELETE RESTRICT',
    'SELECT 1'
);

PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

-- Database-level examination scope enforcement.
--
-- Every examination gets one scope row per stream it covers.
-- An All Streams examination therefore expands to all active streams
-- belonging to its class, while a stream-specific examination gets one row.
--
-- The UNIQUE constraint is the final database-level protection:
-- for the same academic year, term, class and examination type,
-- two examination scopes cannot claim the same stream.
-- Examination name is deliberately NOT part of uniqueness; it is a
-- human-entered label and may be reused.
--
-- This makes these combinations impossible:
--   All Streams + South
--   South + All Streams
--   South + South
--
-- while allowing:
--   South + West
--   South + East
--   West + North

CREATE TABLE IF NOT EXISTS examination_stream_scopes (
    examination_scope_id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    examination_id INT(10) UNSIGNED NOT NULL,
    academic_year_id INT(10) UNSIGNED NOT NULL,
    term_id INT(10) UNSIGNED NOT NULL,
    class_id INT(10) UNSIGNED NOT NULL,
    examination_name VARCHAR(100) NOT NULL,
    examination_type VARCHAR(50) NOT NULL DEFAULT '',
    stream_id INT(10) UNSIGNED NOT NULL,

    PRIMARY KEY (examination_scope_id),

    UNIQUE KEY uq_examination_stream_scope (
        academic_year_id,
        term_id,
        class_id,
        examination_type,
        stream_id
    ),

    KEY idx_examination_scope_exam (
        examination_id
    ),

    CONSTRAINT fk_examination_scope_exam
        FOREIGN KEY (examination_id)
        REFERENCES examinations(examination_id)
        ON UPDATE CASCADE
        ON DELETE CASCADE,

    CONSTRAINT fk_examination_scope_stream
        FOREIGN KEY (stream_id)
        REFERENCES streams(stream_id)
        ON UPDATE CASCADE
        ON DELETE RESTRICT
);

-- Normalize the scope uniqueness rule if this migration was
-- already run with the earlier examination-name-based constraint.
SET @has_scope_unique := (
    SELECT COUNT(*)
    FROM information_schema.STATISTICS
    WHERE TABLE_SCHEMA = DATABASE()
      AND TABLE_NAME = 'examination_stream_scopes'
      AND INDEX_NAME = 'uq_examination_stream_scope'
);

SET @sql := IF(
    @has_scope_unique > 0,
    'ALTER TABLE examination_stream_scopes DROP INDEX uq_examination_stream_scope',
    'SELECT 1'
);

PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

SET @has_new_scope_unique := (
    SELECT COUNT(*)
    FROM information_schema.STATISTICS
    WHERE TABLE_SCHEMA = DATABASE()
      AND TABLE_NAME = 'examination_stream_scopes'
      AND INDEX_NAME = 'uq_examination_stream_scope'
);

SET @sql := IF(
    @has_new_scope_unique = 0,
    'ALTER TABLE examination_stream_scopes ADD UNIQUE KEY uq_examination_stream_scope (academic_year_id, term_id, class_id, examination_type, stream_id)',
    'SELECT 1'
);

PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

-- Seed scope rows for examinations that already exist.
-- INSERT IGNORE keeps the migration safely re-runnable.
INSERT IGNORE INTO examination_stream_scopes (
    examination_id,
    academic_year_id,
    term_id,
    class_id,
    examination_name,
    examination_type,
    stream_id
)
SELECT
    e.examination_id,
    e.academic_year_id,
    e.term_id,
    e.class_id,
    TRIM(e.examination_name),
    TRIM(COALESCE(e.examination_type, '')),
    s.stream_id
FROM examinations e
INNER JOIN streams s
    ON s.class_id = e.class_id
   AND s.status = 'Active'
WHERE e.stream_id IS NULL;

INSERT IGNORE INTO examination_stream_scopes (
    examination_id,
    academic_year_id,
    term_id,
    class_id,
    examination_name,
    examination_type,
    stream_id
)
SELECT
    e.examination_id,
    e.academic_year_id,
    e.term_id,
    e.class_id,
    TRIM(e.examination_name),
    TRIM(COALESCE(e.examination_type, '')),
    e.stream_id
FROM examinations e
WHERE e.stream_id IS NOT NULL;

