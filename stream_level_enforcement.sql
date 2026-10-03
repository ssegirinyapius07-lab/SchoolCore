-- Enforce the secondary-school stream rules:
-- O-Level  -> West, South, East, North
-- A-Level  -> Sciences, Arts
--
-- Run this once in schoolcore_db after the academic level/class migration.
-- Existing rows are not deleted. The triggers prevent invalid new or changed rows.
-- Existing invalid rows can be found with the diagnostic query at the end.

DELIMITER $$

DROP TRIGGER IF EXISTS trg_streams_validate_level_insert$$

CREATE TRIGGER trg_streams_validate_level_insert
BEFORE INSERT ON streams
FOR EACH ROW
BEGIN
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
END$$

DROP TRIGGER IF EXISTS trg_streams_validate_level_update$$

CREATE TRIGGER trg_streams_validate_level_update
BEFORE UPDATE ON streams
FOR EACH ROW
BEGIN
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
END$$

DROP TRIGGER IF EXISTS trg_enrollments_validate_stream_insert$$

CREATE TRIGGER trg_enrollments_validate_stream_insert
BEFORE INSERT ON enrollments
FOR EACH ROW
BEGIN
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
END$$

DROP TRIGGER IF EXISTS trg_enrollments_validate_stream_update$$

CREATE TRIGGER trg_enrollments_validate_stream_update
BEFORE UPDATE ON enrollments
FOR EACH ROW
BEGIN
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
END$$

DELIMITER ;

-- Diagnostic: review any existing stream names that do not match the rules.
SELECT
    s.stream_id,
    s.class_id,
    c.class_name,
    al.level_code,
    s.stream_name,
    s.status
FROM streams s
INNER JOIN classes c
    ON c.class_id = s.class_id
INNER JOIN academic_levels al
    ON al.academic_level_id = c.academic_level_id
WHERE
    (al.level_code = 'O_LEVEL'
        AND s.stream_name NOT IN ('West', 'South', 'East', 'North'))
    OR
    (al.level_code = 'A_LEVEL'
        AND s.stream_name NOT IN ('Sciences', 'Arts'))
ORDER BY c.class_id, s.stream_name;
