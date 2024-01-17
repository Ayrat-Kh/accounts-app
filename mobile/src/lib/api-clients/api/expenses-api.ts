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


import type { Configuration } from '../configuration';
import type { AxiosPromise, AxiosInstance, RawAxiosRequestConfig } from 'axios';
import globalAxios from 'axios';
// Some imports not used depending on template conditions
// @ts-ignore
import { DUMMY_BASE_URL, assertParamExists, setApiKeyToObject, setBasicAuthToObject, setBearerAuthToObject, setOAuthToObject, setSearchParams, serializeDataIfNeeded, toPathString, createRequestFunction } from '../common';
// @ts-ignore
import { BASE_PATH, COLLECTION_FORMATS, RequestArgs, BaseAPI, RequiredError, operationServerMap } from '../base';
// @ts-ignore
import { ExpensesExpenseResult } from '../models';
// @ts-ignore
import { ExpensesExpenseUpsertDto } from '../models';
// @ts-ignore
import { ExpensesExpensesResult } from '../models';
// @ts-ignore
import { HelpersErrorResponse } from '../models';
/**
 * ExpensesApi - axios parameter creator
 * @export
 */
export const ExpensesApiAxiosParamCreator = function (configuration?: Configuration) {
    return {
        /**
         * Get expense by expenseId
         * @summary Get expense by expenseId
         * @param {string} authorization Bearer
         * @param {string} expenseId :expenseId
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        v1ExpensesExpenseIdGet: async (authorization: string, expenseId: string, options: RawAxiosRequestConfig = {}): Promise<RequestArgs> => {
            // verify required parameter 'authorization' is not null or undefined
            assertParamExists('v1ExpensesExpenseIdGet', 'authorization', authorization)
            // verify required parameter 'expenseId' is not null or undefined
            assertParamExists('v1ExpensesExpenseIdGet', 'expenseId', expenseId)
            const localVarPath = `/v1/expenses/:expenseId`
                .replace(`{${"expenseId"}}`, encodeURIComponent(String(expenseId)));
            // use dummy base URL string because the URL constructor only accepts absolute URLs.
            const localVarUrlObj = new URL(localVarPath, DUMMY_BASE_URL);
            let baseOptions;
            if (configuration) {
                baseOptions = configuration.baseOptions;
            }

            const localVarRequestOptions = { method: 'GET', ...baseOptions, ...options};
            const localVarHeaderParameter = {} as any;
            const localVarQueryParameter = {} as any;

            if (authorization != null) {
                localVarHeaderParameter['Authorization'] = String(authorization);
            }


    
            setSearchParams(localVarUrlObj, localVarQueryParameter);
            let headersFromBaseOptions = baseOptions && baseOptions.headers ? baseOptions.headers : {};
            localVarRequestOptions.headers = {...localVarHeaderParameter, ...headersFromBaseOptions, ...options.headers};

            return {
                url: toPathString(localVarUrlObj),
                options: localVarRequestOptions,
            };
        },
        /**
         * Update an expense for a logged in user
         * @summary Update an expense for a logged in user
         * @param {string} authorization Bearer
         * @param {string} expenseId :expenseId
         * @param {ExpensesExpenseUpsertDto} data Update expense model
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        v1ExpensesExpenseIdPut: async (authorization: string, expenseId: string, data: ExpensesExpenseUpsertDto, options: RawAxiosRequestConfig = {}): Promise<RequestArgs> => {
            // verify required parameter 'authorization' is not null or undefined
            assertParamExists('v1ExpensesExpenseIdPut', 'authorization', authorization)
            // verify required parameter 'expenseId' is not null or undefined
            assertParamExists('v1ExpensesExpenseIdPut', 'expenseId', expenseId)
            // verify required parameter 'data' is not null or undefined
            assertParamExists('v1ExpensesExpenseIdPut', 'data', data)
            const localVarPath = `/v1/expenses/:expenseId`
                .replace(`{${"expenseId"}}`, encodeURIComponent(String(expenseId)));
            // use dummy base URL string because the URL constructor only accepts absolute URLs.
            const localVarUrlObj = new URL(localVarPath, DUMMY_BASE_URL);
            let baseOptions;
            if (configuration) {
                baseOptions = configuration.baseOptions;
            }

            const localVarRequestOptions = { method: 'PUT', ...baseOptions, ...options};
            const localVarHeaderParameter = {} as any;
            const localVarQueryParameter = {} as any;

            if (authorization != null) {
                localVarHeaderParameter['Authorization'] = String(authorization);
            }


    
            localVarHeaderParameter['Content-Type'] = 'application/json';

            setSearchParams(localVarUrlObj, localVarQueryParameter);
            let headersFromBaseOptions = baseOptions && baseOptions.headers ? baseOptions.headers : {};
            localVarRequestOptions.headers = {...localVarHeaderParameter, ...headersFromBaseOptions, ...options.headers};
            localVarRequestOptions.data = serializeDataIfNeeded(data, localVarRequestOptions, configuration)

            return {
                url: toPathString(localVarUrlObj),
                options: localVarRequestOptions,
            };
        },
        /**
         * Get expenses for the logged in user
         * @summary Get expenses for the logged in user
         * @param {string} authorization Bearer
         * @param {number} page page or 1 by default
         * @param {number} pageSize pageSize or 300 by default
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        v1ExpensesGet: async (authorization: string, page: number, pageSize: number, options: RawAxiosRequestConfig = {}): Promise<RequestArgs> => {
            // verify required parameter 'authorization' is not null or undefined
            assertParamExists('v1ExpensesGet', 'authorization', authorization)
            // verify required parameter 'page' is not null or undefined
            assertParamExists('v1ExpensesGet', 'page', page)
            // verify required parameter 'pageSize' is not null or undefined
            assertParamExists('v1ExpensesGet', 'pageSize', pageSize)
            const localVarPath = `/v1/expenses`;
            // use dummy base URL string because the URL constructor only accepts absolute URLs.
            const localVarUrlObj = new URL(localVarPath, DUMMY_BASE_URL);
            let baseOptions;
            if (configuration) {
                baseOptions = configuration.baseOptions;
            }

            const localVarRequestOptions = { method: 'GET', ...baseOptions, ...options};
            const localVarHeaderParameter = {} as any;
            const localVarQueryParameter = {} as any;

            if (page !== undefined) {
                localVarQueryParameter['page'] = page;
            }

            if (pageSize !== undefined) {
                localVarQueryParameter['pageSize'] = pageSize;
            }

            if (authorization != null) {
                localVarHeaderParameter['Authorization'] = String(authorization);
            }


    
            setSearchParams(localVarUrlObj, localVarQueryParameter);
            let headersFromBaseOptions = baseOptions && baseOptions.headers ? baseOptions.headers : {};
            localVarRequestOptions.headers = {...localVarHeaderParameter, ...headersFromBaseOptions, ...options.headers};

            return {
                url: toPathString(localVarUrlObj),
                options: localVarRequestOptions,
            };
        },
        /**
         * Create an expense for a logged in user
         * @summary Create an expense for a logged in user
         * @param {string} authorization Bearer
         * @param {ExpensesExpenseUpsertDto} data Update expense model
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        v1ExpensesPost: async (authorization: string, data: ExpensesExpenseUpsertDto, options: RawAxiosRequestConfig = {}): Promise<RequestArgs> => {
            // verify required parameter 'authorization' is not null or undefined
            assertParamExists('v1ExpensesPost', 'authorization', authorization)
            // verify required parameter 'data' is not null or undefined
            assertParamExists('v1ExpensesPost', 'data', data)
            const localVarPath = `/v1/expenses`;
            // use dummy base URL string because the URL constructor only accepts absolute URLs.
            const localVarUrlObj = new URL(localVarPath, DUMMY_BASE_URL);
            let baseOptions;
            if (configuration) {
                baseOptions = configuration.baseOptions;
            }

            const localVarRequestOptions = { method: 'POST', ...baseOptions, ...options};
            const localVarHeaderParameter = {} as any;
            const localVarQueryParameter = {} as any;

            if (authorization != null) {
                localVarHeaderParameter['Authorization'] = String(authorization);
            }


    
            localVarHeaderParameter['Content-Type'] = 'application/json';

            setSearchParams(localVarUrlObj, localVarQueryParameter);
            let headersFromBaseOptions = baseOptions && baseOptions.headers ? baseOptions.headers : {};
            localVarRequestOptions.headers = {...localVarHeaderParameter, ...headersFromBaseOptions, ...options.headers};
            localVarRequestOptions.data = serializeDataIfNeeded(data, localVarRequestOptions, configuration)

            return {
                url: toPathString(localVarUrlObj),
                options: localVarRequestOptions,
            };
        },
    }
};

/**
 * ExpensesApi - functional programming interface
 * @export
 */
export const ExpensesApiFp = function(configuration?: Configuration) {
    const localVarAxiosParamCreator = ExpensesApiAxiosParamCreator(configuration)
    return {
        /**
         * Get expense by expenseId
         * @summary Get expense by expenseId
         * @param {string} authorization Bearer
         * @param {string} expenseId :expenseId
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        async v1ExpensesExpenseIdGet(authorization: string, expenseId: string, options?: RawAxiosRequestConfig): Promise<(axios?: AxiosInstance, basePath?: string) => AxiosPromise<ExpensesExpenseResult>> {
            const localVarAxiosArgs = await localVarAxiosParamCreator.v1ExpensesExpenseIdGet(authorization, expenseId, options);
            const index = configuration?.serverIndex ?? 0;
            const operationBasePath = operationServerMap['ExpensesApi.v1ExpensesExpenseIdGet']?.[index]?.url;
            return (axios, basePath) => createRequestFunction(localVarAxiosArgs, globalAxios, BASE_PATH, configuration)(axios, operationBasePath || basePath);
        },
        /**
         * Update an expense for a logged in user
         * @summary Update an expense for a logged in user
         * @param {string} authorization Bearer
         * @param {string} expenseId :expenseId
         * @param {ExpensesExpenseUpsertDto} data Update expense model
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        async v1ExpensesExpenseIdPut(authorization: string, expenseId: string, data: ExpensesExpenseUpsertDto, options?: RawAxiosRequestConfig): Promise<(axios?: AxiosInstance, basePath?: string) => AxiosPromise<ExpensesExpenseResult>> {
            const localVarAxiosArgs = await localVarAxiosParamCreator.v1ExpensesExpenseIdPut(authorization, expenseId, data, options);
            const index = configuration?.serverIndex ?? 0;
            const operationBasePath = operationServerMap['ExpensesApi.v1ExpensesExpenseIdPut']?.[index]?.url;
            return (axios, basePath) => createRequestFunction(localVarAxiosArgs, globalAxios, BASE_PATH, configuration)(axios, operationBasePath || basePath);
        },
        /**
         * Get expenses for the logged in user
         * @summary Get expenses for the logged in user
         * @param {string} authorization Bearer
         * @param {number} page page or 1 by default
         * @param {number} pageSize pageSize or 300 by default
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        async v1ExpensesGet(authorization: string, page: number, pageSize: number, options?: RawAxiosRequestConfig): Promise<(axios?: AxiosInstance, basePath?: string) => AxiosPromise<ExpensesExpensesResult>> {
            const localVarAxiosArgs = await localVarAxiosParamCreator.v1ExpensesGet(authorization, page, pageSize, options);
            const index = configuration?.serverIndex ?? 0;
            const operationBasePath = operationServerMap['ExpensesApi.v1ExpensesGet']?.[index]?.url;
            return (axios, basePath) => createRequestFunction(localVarAxiosArgs, globalAxios, BASE_PATH, configuration)(axios, operationBasePath || basePath);
        },
        /**
         * Create an expense for a logged in user
         * @summary Create an expense for a logged in user
         * @param {string} authorization Bearer
         * @param {ExpensesExpenseUpsertDto} data Update expense model
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        async v1ExpensesPost(authorization: string, data: ExpensesExpenseUpsertDto, options?: RawAxiosRequestConfig): Promise<(axios?: AxiosInstance, basePath?: string) => AxiosPromise<ExpensesExpenseResult>> {
            const localVarAxiosArgs = await localVarAxiosParamCreator.v1ExpensesPost(authorization, data, options);
            const index = configuration?.serverIndex ?? 0;
            const operationBasePath = operationServerMap['ExpensesApi.v1ExpensesPost']?.[index]?.url;
            return (axios, basePath) => createRequestFunction(localVarAxiosArgs, globalAxios, BASE_PATH, configuration)(axios, operationBasePath || basePath);
        },
    }
};

/**
 * ExpensesApi - factory interface
 * @export
 */
export const ExpensesApiFactory = function (configuration?: Configuration, basePath?: string, axios?: AxiosInstance) {
    const localVarFp = ExpensesApiFp(configuration)
    return {
        /**
         * Get expense by expenseId
         * @summary Get expense by expenseId
         * @param {string} authorization Bearer
         * @param {string} expenseId :expenseId
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        v1ExpensesExpenseIdGet(authorization: string, expenseId: string, options?: any): AxiosPromise<ExpensesExpenseResult> {
            return localVarFp.v1ExpensesExpenseIdGet(authorization, expenseId, options).then((request) => request(axios, basePath));
        },
        /**
         * Update an expense for a logged in user
         * @summary Update an expense for a logged in user
         * @param {string} authorization Bearer
         * @param {string} expenseId :expenseId
         * @param {ExpensesExpenseUpsertDto} data Update expense model
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        v1ExpensesExpenseIdPut(authorization: string, expenseId: string, data: ExpensesExpenseUpsertDto, options?: any): AxiosPromise<ExpensesExpenseResult> {
            return localVarFp.v1ExpensesExpenseIdPut(authorization, expenseId, data, options).then((request) => request(axios, basePath));
        },
        /**
         * Get expenses for the logged in user
         * @summary Get expenses for the logged in user
         * @param {string} authorization Bearer
         * @param {number} page page or 1 by default
         * @param {number} pageSize pageSize or 300 by default
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        v1ExpensesGet(authorization: string, page: number, pageSize: number, options?: any): AxiosPromise<ExpensesExpensesResult> {
            return localVarFp.v1ExpensesGet(authorization, page, pageSize, options).then((request) => request(axios, basePath));
        },
        /**
         * Create an expense for a logged in user
         * @summary Create an expense for a logged in user
         * @param {string} authorization Bearer
         * @param {ExpensesExpenseUpsertDto} data Update expense model
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        v1ExpensesPost(authorization: string, data: ExpensesExpenseUpsertDto, options?: any): AxiosPromise<ExpensesExpenseResult> {
            return localVarFp.v1ExpensesPost(authorization, data, options).then((request) => request(axios, basePath));
        },
    };
};

/**
 * ExpensesApi - object-oriented interface
 * @export
 * @class ExpensesApi
 * @extends {BaseAPI}
 */
export class ExpensesApi extends BaseAPI {
    /**
     * Get expense by expenseId
     * @summary Get expense by expenseId
     * @param {string} authorization Bearer
     * @param {string} expenseId :expenseId
     * @param {*} [options] Override http request option.
     * @throws {RequiredError}
     * @memberof ExpensesApi
     */
    public v1ExpensesExpenseIdGet(authorization: string, expenseId: string, options?: RawAxiosRequestConfig) {
        return ExpensesApiFp(this.configuration).v1ExpensesExpenseIdGet(authorization, expenseId, options).then((request) => request(this.axios, this.basePath));
    }

    /**
     * Update an expense for a logged in user
     * @summary Update an expense for a logged in user
     * @param {string} authorization Bearer
     * @param {string} expenseId :expenseId
     * @param {ExpensesExpenseUpsertDto} data Update expense model
     * @param {*} [options] Override http request option.
     * @throws {RequiredError}
     * @memberof ExpensesApi
     */
    public v1ExpensesExpenseIdPut(authorization: string, expenseId: string, data: ExpensesExpenseUpsertDto, options?: RawAxiosRequestConfig) {
        return ExpensesApiFp(this.configuration).v1ExpensesExpenseIdPut(authorization, expenseId, data, options).then((request) => request(this.axios, this.basePath));
    }

    /**
     * Get expenses for the logged in user
     * @summary Get expenses for the logged in user
     * @param {string} authorization Bearer
     * @param {number} page page or 1 by default
     * @param {number} pageSize pageSize or 300 by default
     * @param {*} [options] Override http request option.
     * @throws {RequiredError}
     * @memberof ExpensesApi
     */
    public v1ExpensesGet(authorization: string, page: number, pageSize: number, options?: RawAxiosRequestConfig) {
        return ExpensesApiFp(this.configuration).v1ExpensesGet(authorization, page, pageSize, options).then((request) => request(this.axios, this.basePath));
    }

    /**
     * Create an expense for a logged in user
     * @summary Create an expense for a logged in user
     * @param {string} authorization Bearer
     * @param {ExpensesExpenseUpsertDto} data Update expense model
     * @param {*} [options] Override http request option.
     * @throws {RequiredError}
     * @memberof ExpensesApi
     */
    public v1ExpensesPost(authorization: string, data: ExpensesExpenseUpsertDto, options?: RawAxiosRequestConfig) {
        return ExpensesApiFp(this.configuration).v1ExpensesPost(authorization, data, options).then((request) => request(this.axios, this.basePath));
    }
}

