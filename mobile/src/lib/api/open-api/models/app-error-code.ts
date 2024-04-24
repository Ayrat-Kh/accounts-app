/* tslint:disable */
/* eslint-disable */
/**
 * Accounts API
 * No description provided (generated by Openapi Generator https://github.com/openapitools/openapi-generator)
 *
 * The version of the OpenAPI document: 1.0
 * Contact: aira3t@gmail.com
 *
 * NOTE: This class is auto generated by OpenAPI Generator (https://openapi-generator.tech).
 * https://openapi-generator.tech
 * Do not edit the class manually.
 */



/**
 * 
 * @export
 * @enum {string}
 */

export const AppErrorCode = {
    AppError: 'APP_ERROR',
    InvalidInput: 'INVALID_INPUT',
    DbNotFound: 'DB_NOT_FOUND',
    DbInsertError: 'DB_INSERT_ERROR',
    DbQueryError: 'DB_QUERY_ERROR',
    ThirdPartyRequest: 'THIRD_PARTY_REQUEST',
    ValidationError: 'VALIDATION_ERROR',
    ParseBodyError: 'PARSE_BODY_ERROR'
} as const;

export type AppErrorCode = typeof AppErrorCode[keyof typeof AppErrorCode];


