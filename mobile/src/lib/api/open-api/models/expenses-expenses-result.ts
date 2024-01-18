/* tslint:disable */
/* eslint-disable */
/**
 * Expenso-app API
 * No description provided (generated by Openapi Generator https://github.com/openapitools/openapi-generator)
 *
 * The version of the OpenAPI document: 1.0
 * Contact: fiber@swagger.io
 *
 * NOTE: This class is auto generated by OpenAPI Generator (https://openapi-generator.tech).
 * https://openapi-generator.tech
 * Do not edit the class manually.
 */


// May contain unused imports in some cases
// @ts-ignore
import { ExpensesExpenseDto } from './expenses-expense-dto';
// May contain unused imports in some cases
// @ts-ignore
import { HelpersPaginationResult } from './helpers-pagination-result';

/**
 * 
 * @export
 * @interface ExpensesExpensesResult
 */
export interface ExpensesExpensesResult {
    /**
     * 
     * @type {Array<ExpensesExpenseDto>}
     * @memberof ExpensesExpensesResult
     */
    'expenses': Array<ExpensesExpenseDto>;
    /**
     * 
     * @type {HelpersPaginationResult}
     * @memberof ExpensesExpensesResult
     */
    'paginationResult': HelpersPaginationResult;
}

