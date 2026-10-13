-- Users who signed in with Google. Only allowed e-mails ever get a row.
CREATE TABLE users (
    id          uuid PRIMARY KEY DEFAULT gen_random_uuid(),
    google_sub  text NOT NULL UNIQUE,
    email       text NOT NULL,
    created_at  timestamptz NOT NULL DEFAULT now()
);

-- Opaque refresh tokens. Only a SHA-256 hash is stored, so a database leak
-- does not leak usable tokens. Rotation revokes the old row and inserts a new
-- one; absolute_expires_at caps the whole chain.
CREATE TABLE refresh_tokens (
    id                   uuid PRIMARY KEY DEFAULT gen_random_uuid(),
    user_id              uuid NOT NULL REFERENCES users (id) ON DELETE CASCADE,
    token_hash           text NOT NULL UNIQUE,
    expires_at           timestamptz NOT NULL,
    absolute_expires_at  timestamptz NOT NULL,
    revoked_at           timestamptz,
    created_at           timestamptz NOT NULL DEFAULT now()
);
CREATE INDEX refresh_tokens_user_idx ON refresh_tokens (user_id);

-- One scanned receipt: the price and, optionally, the photo stored in Garage.
-- Money is an integer number of minor units (cents) to avoid floating point.
CREATE TABLE receipts (
    id                  uuid PRIMARY KEY DEFAULT gen_random_uuid(),
    user_id             uuid NOT NULL REFERENCES users (id) ON DELETE CASCADE,
    amount_minor        bigint NOT NULL CHECK (amount_minor >= 0),
    currency            char(3) NOT NULL CHECK (currency ~ '^[A-Z]{3}$'),
    purchased_at        timestamptz NOT NULL,
    merchant            text CHECK (char_length(merchant) <= 200),
    ocr_confidence      real CHECK (ocr_confidence BETWEEN 0 AND 1),
    image_object_key    text,
    image_sha256        text,
    image_size_bytes    bigint,
    image_mime_type     text,
    created_at          timestamptz NOT NULL DEFAULT now(),
    updated_at          timestamptz NOT NULL DEFAULT now(),
    deleted_at          timestamptz,
    -- Set once the receipt has been written to the Obsidian vault (see docs/obsidian-sync.md).
    obsidian_synced_at  timestamptz
);
-- Serves the list endpoint's keyset pagination: newest purchase first.
CREATE INDEX receipts_user_purchased_idx ON receipts (user_id, purchased_at DESC, id DESC)
    WHERE deleted_at IS NULL;
CREATE INDEX receipts_obsidian_pending_idx ON receipts (created_at)
    WHERE obsidian_synced_at IS NULL AND deleted_at IS NULL;

-- Formats a timestamp as RFC 3339 in UTC with milliseconds, the format the
-- API speaks. Every SELECT uses it, so the C++ side parses one format only.
CREATE FUNCTION rfc3339(ts timestamptz) RETURNS text
    LANGUAGE sql STABLE STRICT
    RETURN to_char(ts AT TIME ZONE 'UTC', 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"');
