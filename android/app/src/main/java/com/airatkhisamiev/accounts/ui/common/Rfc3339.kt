package com.airatkhisamiev.accounts.ui.common

// The core speaks RFC 3339 timestamps ("2026-10-13T12:00:00.000Z"); the UI
// only ever shows their date or month, which are fixed-length prefixes.

/** "2026-10-13T12:00:00.000Z" -> "2026-10-13" */
fun rfc3339Date(timestamp: String): String = timestamp.take(DATE_LENGTH)

/** "2026-10-13T12:00:00.000Z" -> "2026-10" */
fun rfc3339Month(timestamp: String): String = timestamp.take(MONTH_LENGTH)

private const val DATE_LENGTH = 10
private const val MONTH_LENGTH = 7
