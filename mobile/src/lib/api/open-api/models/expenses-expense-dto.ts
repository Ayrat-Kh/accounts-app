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
import { ExpensesExpenseItemDto } from './expenses-expense-item-dto';
// May contain unused imports in some cases
// @ts-ignore
import { ExpensesGeoCoord } from './expenses-geo-coord';

/**
 * 
 * @export
 * @interface ExpensesExpenseDto
 */
export interface ExpensesExpenseDto {
    /**
     * 
     * @type {string}
     * @memberof ExpensesExpenseDto
     */
    'category': string;
    /**
     * 
     * @type {ExpensesGeoCoord}
     * @memberof ExpensesExpenseDto
     */
    'coord'?: ExpensesGeoCoord;
    /**
     * 
     * @type {number}
     * @memberof ExpensesExpenseDto
     */
    'createdAt': number;
    /**
     * 
     * @type {string}
     * @memberof ExpensesExpenseDto
     */
    'currencyCode': string;
    /**
     * 
     * @type {string}
     * @memberof ExpensesExpenseDto
     */
    'date'?: string;
    /**
     * 
     * @type {Array<ExpensesExpenseItemDto>}
     * @memberof ExpensesExpenseDto
     */
    'details': Array<ExpensesExpenseItemDto>;
    /**
     * 
     * @type {string}
     * @memberof ExpensesExpenseDto
     */
    'id': string;
    /**
     * 
     * @type {string}
     * @memberof ExpensesExpenseDto
     */
    'name': string;
    /**
     * 
     * @type {number}
     * @memberof ExpensesExpenseDto
     */
    'total': number;
    /**
     * 
     * @type {number}
     * @memberof ExpensesExpenseDto
     */
    'updatedAt': number;
    /**
     * 
     * @type {string}
     * @memberof ExpensesExpenseDto
     */
    'userId': string;
}

