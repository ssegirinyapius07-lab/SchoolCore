-- SchoolCore academic-period security and audit foundation
-- Idempotent migration. Safe to run more than once.

CREATE TABLE IF NOT EXISTS academic_edit_approvals (
    approval_id BIGINT(20) UNSIGNED NOT NULL AUTO_INCREMENT,
    academic_year_id INT(10) UNSIGNED NOT NULL,
    term_id INT(10) UNSIGNED DEFAULT NULL,
    operation VARCHAR(30) NOT NULL,
    table_name VARCHAR(100) NOT NULL,
    record_id VARCHAR(100) DEFAULT NULL,
    reason TEXT NOT NULL,
    requested_by INT(10) UNSIGNED NOT NULL,
    approved_by INT(10) UNSIGNED DEFAULT NULL,
    status ENUM('Pending','Approved','Rejected','Cancelled') NOT NULL DEFAULT 'Pending',
    requested_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    approved_at DATETIME DEFAULT NULL,

    PRIMARY KEY (approval_id),
    KEY idx_academic_edit_approvals_year (academic_year_id),
    KEY idx_academic_edit_approvals_status (status),
    KEY idx_academic_edit_approvals_requester (requested_by),
    KEY idx_academic_edit_approvals_approver (approved_by),

    CONSTRAINT fk_academic_edit_approval_year
        FOREIGN KEY (academic_year_id)
        REFERENCES academic_years(academic_year_id)
        ON UPDATE CASCADE
        ON DELETE RESTRICT,

    CONSTRAINT fk_academic_edit_approval_term
        FOREIGN KEY (term_id)
        REFERENCES terms(term_id)
        ON UPDATE CASCADE
        ON DELETE RESTRICT,

    CONSTRAINT fk_academic_edit_approval_requester
        FOREIGN KEY (requested_by)
        REFERENCES users(user_id)
        ON UPDATE CASCADE
        ON DELETE RESTRICT,

    CONSTRAINT fk_academic_edit_approval_approver
        FOREIGN KEY (approved_by)
        REFERENCES users(user_id)
        ON UPDATE CASCADE
        ON DELETE RESTRICT
);

SET @has_audit_reason := (
    SELECT COUNT(*)
    FROM information_schema.COLUMNS
    WHERE TABLE_SCHEMA = DATABASE()
      AND TABLE_NAME = 'audit_logs'
      AND COLUMN_NAME = 'reason'
);

SET @sql := IF(
    @has_audit_reason = 0,
    'ALTER TABLE audit_logs ADD COLUMN reason TEXT NULL AFTER description',
    'SELECT 1'
);

PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

SET @has_audit_approval := (
    SELECT COUNT(*)
    FROM information_schema.COLUMNS
    WHERE TABLE_SCHEMA = DATABASE()
      AND TABLE_NAME = 'audit_logs'
      AND COLUMN_NAME = 'approval_id'
);

SET @sql := IF(
    @has_audit_approval = 0,
    'ALTER TABLE audit_logs ADD COLUMN approval_id BIGINT(20) UNSIGNED NULL AFTER reason',
    'SELECT 1'
);

PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

SET @has_audit_old_values := (
    SELECT COUNT(*)
    FROM information_schema.COLUMNS
    WHERE TABLE_SCHEMA = DATABASE()
      AND TABLE_NAME = 'audit_logs'
      AND COLUMN_NAME = 'old_values'
);

SET @sql := IF(
    @has_audit_old_values = 0,
    'ALTER TABLE audit_logs ADD COLUMN old_values LONGTEXT NULL AFTER approval_id',
    'SELECT 1'
);

PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

SET @has_audit_new_values := (
    SELECT COUNT(*)
    FROM information_schema.COLUMNS
    WHERE TABLE_SCHEMA = DATABASE()
      AND TABLE_NAME = 'audit_logs'
      AND COLUMN_NAME = 'new_values'
);

SET @sql := IF(
    @has_audit_new_values = 0,
    'ALTER TABLE audit_logs ADD COLUMN new_values LONGTEXT NULL AFTER old_values',
    'SELECT 1'
);

PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

SET @has_audit_approval_key := (
    SELECT COUNT(*)
    FROM information_schema.STATISTICS
    WHERE TABLE_SCHEMA = DATABASE()
      AND TABLE_NAME = 'audit_logs'
      AND INDEX_NAME = 'idx_audit_logs_approval'
);

SET @sql := IF(
    @has_audit_approval_key = 0,
    'ALTER TABLE audit_logs ADD KEY idx_audit_logs_approval (approval_id)',
    'SELECT 1'
);

PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;

SET @has_override_permission := (
    SELECT COUNT(*)
    FROM permissions
    WHERE permission_name = 'academic_records.override_inactive_year'
);

INSERT INTO permissions (
    permission_name,
    description
)
SELECT
    'academic_records.override_inactive_year',
    'Request and approve controlled edits to records belonging to inactive academic years.'
WHERE @has_override_permission = 0;

SET @override_permission_id := (
    SELECT permission_id
    FROM permissions
    WHERE permission_name = 'academic_records.override_inactive_year'
    LIMIT 1
);

-- Give the controlled override permission to administrative/academic supervisory roles.
INSERT IGNORE INTO role_permissions (role_id, permission_id)
SELECT r.role_id, @override_permission_id
FROM roles r
WHERE r.role_name IN ('Administrator', 'Academic Registrar', 'Head Teacher');

-- Back-reference from audit logs to the approval record.
SET @has_audit_approval_fk := (
    SELECT COUNT(*)
    FROM information_schema.KEY_COLUMN_USAGE
    WHERE TABLE_SCHEMA = DATABASE()
      AND TABLE_NAME = 'audit_logs'
      AND CONSTRAINT_NAME = 'fk_audit_logs_approval'
);

SET @sql := IF(
    @has_audit_approval_fk = 0,
    'ALTER TABLE audit_logs ADD CONSTRAINT fk_audit_logs_approval FOREIGN KEY (approval_id) REFERENCES academic_edit_approvals(approval_id) ON UPDATE CASCADE ON DELETE RESTRICT',
    'SELECT 1'
);

PREPARE stmt FROM @sql;
EXECUTE stmt;
DEALLOCATE PREPARE stmt;
