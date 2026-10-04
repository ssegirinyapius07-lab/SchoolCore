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

DROP TRIGGER IF EXISTS trg_examinations_validate_stream_insert;

DELIMITER $$

CREATE TRIGGER trg_examinations_validate_stream_insert
BEFORE INSERT ON examinations
FOR EACH ROW
BEGIN
    DECLARE streamClassId INT UNSIGNED DEFAULT NULL;

    IF NEW.stream_id IS NOT NULL THEN
        SELECT s.class_id
        INTO streamClassId
        FROM streams s
        WHERE s.stream_id = NEW.stream_id
          AND s.status = 'Active'
        LIMIT 1;

        IF streamClassId IS NULL THEN
            SIGNAL SQLSTATE '45000'
                SET MESSAGE_TEXT = 'The selected examination stream does not exist or is inactive.';
        ELSEIF streamClassId <> NEW.class_id THEN
            SIGNAL SQLSTATE '45000'
                SET MESSAGE_TEXT = 'The examination stream must belong to the selected class.';
        END IF;
    END IF;
END$$

DELIMITER ;

DROP TRIGGER IF EXISTS trg_examinations_validate_stream_update;

DELIMITER $$

CREATE TRIGGER trg_examinations_validate_stream_update
BEFORE UPDATE ON examinations
FOR EACH ROW
BEGIN
    DECLARE streamClassId INT UNSIGNED DEFAULT NULL;

    IF NEW.stream_id IS NOT NULL THEN
        SELECT s.class_id
        INTO streamClassId
        FROM streams s
        WHERE s.stream_id = NEW.stream_id
          AND s.status = 'Active'
        LIMIT 1;

        IF streamClassId IS NULL THEN
            SIGNAL SQLSTATE '45000'
                SET MESSAGE_TEXT = 'The selected examination stream does not exist or is inactive.';
        ELSEIF streamClassId <> NEW.class_id THEN
            SIGNAL SQLSTATE '45000'
                SET MESSAGE_TEXT = 'The examination stream must belong to the selected class.';
        END IF;
    END IF;
END$$

DELIMITER ;
