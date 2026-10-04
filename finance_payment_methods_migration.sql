-- ============================================================
-- SchoolCore Finance Payment Methods
-- Uganda-focused payment structure
--
-- Supported payment methods:
--   1. Mobile Money
--   2. Bank
--   3. Online/Electronic Payment
--
-- This migration extends the existing payments table without
-- deleting the legacy payment_method / transaction_reference
-- columns, so existing data remains readable.
-- ============================================================

CREATE TABLE IF NOT EXISTS payment_methods (
    payment_method_id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    method_code VARCHAR(30) NOT NULL,
    method_name VARCHAR(80) NOT NULL,
    description VARCHAR(255) DEFAULT NULL,
    requires_provider TINYINT(1) NOT NULL DEFAULT 1,
    requires_reference TINYINT(1) NOT NULL DEFAULT 1,
    status VARCHAR(20) NOT NULL DEFAULT 'Active',
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (payment_method_id),
    UNIQUE KEY uq_payment_methods_code (method_code),
    UNIQUE KEY uq_payment_methods_name (method_name)
) ENGINE=InnoDB
  DEFAULT CHARSET=utf8mb4
  COLLATE=utf8mb4_unicode_ci;

INSERT INTO payment_methods
    (method_code, method_name, description, requires_provider, requires_reference, status)
VALUES
    (
        'MOBILE_MONEY',
        'Mobile Money',
        'Payments received through supported mobile money services in Uganda.',
        1,
        1,
        'Active'
    ),
    (
        'BANK',
        'Bank',
        'Payments received through a bank deposit, bank transfer or other bank channel.',
        1,
        1,
        'Active'
    ),
    (
        'ONLINE_ELECTRONIC',
        'Online/Electronic Payment',
        'Payments received through an electronic payment gateway, school payment portal or integrated digital channel.',
        1,
        1,
        'Active'
    )
ON DUPLICATE KEY UPDATE
    description = VALUES(description),
    requires_provider = VALUES(requires_provider),
    requires_reference = VALUES(requires_reference),
    status = VALUES(status);

CREATE TABLE IF NOT EXISTS payment_providers (
    payment_provider_id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    payment_method_id INT(10) UNSIGNED NOT NULL,
    provider_code VARCHAR(50) NOT NULL,
    provider_name VARCHAR(120) NOT NULL,
    account_reference VARCHAR(120) DEFAULT NULL,
    status VARCHAR(20) NOT NULL DEFAULT 'Active',
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (payment_provider_id),
    UNIQUE KEY uq_payment_provider_code (provider_code),
    KEY idx_payment_provider_method (payment_method_id),
    CONSTRAINT fk_payment_provider_method
        FOREIGN KEY (payment_method_id)
        REFERENCES payment_methods (payment_method_id)
        ON DELETE RESTRICT
        ON UPDATE CASCADE
) ENGINE=InnoDB
  DEFAULT CHARSET=utf8mb4
  COLLATE=utf8mb4_unicode_ci;

INSERT INTO payment_providers
    (payment_method_id, provider_code, provider_name, status)
SELECT
    pm.payment_method_id,
    'MTN_MOBILE_MONEY',
    'MTN Mobile Money',
    'Active'
FROM payment_methods pm
WHERE pm.method_code = 'MOBILE_MONEY'
  AND NOT EXISTS (
      SELECT 1
      FROM payment_providers pp
      WHERE pp.provider_code = 'MTN_MOBILE_MONEY'
  );

INSERT INTO payment_providers
    (payment_method_id, provider_code, provider_name, status)
SELECT
    pm.payment_method_id,
    'AIRTEL_MONEY',
    'Airtel Money',
    'Active'
FROM payment_methods pm
WHERE pm.method_code = 'MOBILE_MONEY'
  AND NOT EXISTS (
      SELECT 1
      FROM payment_providers pp
      WHERE pp.provider_code = 'AIRTEL_MONEY'
  );

ALTER TABLE payments
    ADD COLUMN IF NOT EXISTS payment_method_id INT(10) UNSIGNED NULL
        AFTER amount,
    ADD COLUMN IF NOT EXISTS payment_provider_id INT(10) UNSIGNED NULL
        AFTER payment_method_id,
    ADD COLUMN IF NOT EXISTS payer_contact VARCHAR(50) NULL
        AFTER payment_provider_id,
    ADD COLUMN IF NOT EXISTS payment_status VARCHAR(30) NOT NULL DEFAULT 'Confirmed'
        AFTER transaction_reference,
    ADD COLUMN IF NOT EXISTS verified_by INT(10) UNSIGNED NULL
        AFTER received_by,
    ADD COLUMN IF NOT EXISTS verified_at DATETIME NULL
        AFTER verified_by;

UPDATE payments p
INNER JOIN payment_methods pm
    ON pm.method_name = p.payment_method
SET p.payment_method_id = pm.payment_method_id
WHERE p.payment_method_id IS NULL
  AND p.payment_method IS NOT NULL;

ALTER TABLE payments
    ADD KEY IF NOT EXISTS idx_payments_method (payment_method_id),
    ADD KEY IF NOT EXISTS idx_payments_provider (payment_provider_id),
    ADD KEY IF NOT EXISTS idx_payments_status (payment_status),
    ADD KEY IF NOT EXISTS idx_payments_verified_by (verified_by);

SET @fk_method_exists = (
    SELECT COUNT(*)
    FROM information_schema.KEY_COLUMN_USAGE
    WHERE TABLE_SCHEMA = DATABASE()
      AND TABLE_NAME = 'payments'
      AND CONSTRAINT_NAME = 'fk_payments_method'
);

SET @sql_method_fk = IF(
    @fk_method_exists = 0,
    'ALTER TABLE payments ADD CONSTRAINT fk_payments_method FOREIGN KEY (payment_method_id) REFERENCES payment_methods(payment_method_id) ON DELETE RESTRICT ON UPDATE CASCADE',
    'SELECT 1'
);

PREPARE stmt_method_fk FROM @sql_method_fk;
EXECUTE stmt_method_fk;
DEALLOCATE PREPARE stmt_method_fk;

SET @fk_provider_exists = (
    SELECT COUNT(*)
    FROM information_schema.KEY_COLUMN_USAGE
    WHERE TABLE_SCHEMA = DATABASE()
      AND TABLE_NAME = 'payments'
      AND CONSTRAINT_NAME = 'fk_payments_provider'
);

SET @sql_provider_fk = IF(
    @fk_provider_exists = 0,
    'ALTER TABLE payments ADD CONSTRAINT fk_payments_provider FOREIGN KEY (payment_provider_id) REFERENCES payment_providers(payment_provider_id) ON DELETE RESTRICT ON UPDATE CASCADE',
    'SELECT 1'
);

PREPARE stmt_provider_fk FROM @sql_provider_fk;
EXECUTE stmt_provider_fk;
DEALLOCATE PREPARE stmt_provider_fk;

SET @fk_verified_exists = (
    SELECT COUNT(*)
    FROM information_schema.KEY_COLUMN_USAGE
    WHERE TABLE_SCHEMA = DATABASE()
      AND TABLE_NAME = 'payments'
      AND CONSTRAINT_NAME = 'fk_payments_verified_by'
);

SET @sql_verified_fk = IF(
    @fk_verified_exists = 0,
    'ALTER TABLE payments ADD CONSTRAINT fk_payments_verified_by FOREIGN KEY (verified_by) REFERENCES users(user_id) ON DELETE SET NULL ON UPDATE CASCADE',
    'SELECT 1'
);

PREPARE stmt_verified_fk FROM @sql_verified_fk;
EXECUTE stmt_verified_fk;
DEALLOCATE PREPARE stmt_verified_fk;

-- IMPORTANT:
-- The existing payment_method and transaction_reference columns are
-- intentionally retained during this transition.
-- New SchoolCore payment screens should use:
--   payment_method_id
--   payment_provider_id
--   payer_contact
--   transaction_reference
--   payment_status
--   verified_by
--   verified_at
--
-- Only these three payment methods should be presented to users:
--   Mobile Money
--   Bank
--   Online/Electronic Payment
