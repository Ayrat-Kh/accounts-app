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


import type { Configuration } from '../configuration';
import type { AxiosPromise, AxiosInstance, RawAxiosRequestConfig } from 'axios';
import globalAxios from 'axios';
// Some imports not used depending on template conditions
// @ts-ignore
import { DUMMY_BASE_URL, assertParamExists, setApiKeyToObject, setBasicAuthToObject, setBearerAuthToObject, setOAuthToObject, setSearchParams, serializeDataIfNeeded, toPathString, createRequestFunction } from '../common';
// @ts-ignore
import { BASE_PATH, COLLECTION_FORMATS, RequestArgs, BaseAPI, RequiredError, operationServerMap } from '../base';
// @ts-ignore
import { AccountResult } from '../models';
// @ts-ignore
import { ErrorResponse } from '../models';
// @ts-ignore
import { UpsertAccount } from '../models';
// @ts-ignore
import { UserAccountsResult } from '../models';
/**
 * AccountsApi - axios parameter creator
 * @export
 */
export const AccountsApiAxiosParamCreator = function (configuration?: Configuration) {
    return {
        /**
         * Update an account for a logged in user
         * @summary Update an account for a logged in user
         * @param {UpsertAccount} data Update account model
         * @param {string} authorization Bearer
         * @param {string} accountId accountId
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        v1AccountsAccountIdPut: async (data: UpsertAccount, authorization: string, accountId: string, options: RawAxiosRequestConfig = {}): Promise<RequestArgs> => {
            // verify required parameter 'data' is not null or undefined
            assertParamExists('v1AccountsAccountIdPut', 'data', data)
            // verify required parameter 'authorization' is not null or undefined
            assertParamExists('v1AccountsAccountIdPut', 'authorization', authorization)
            // verify required parameter 'accountId' is not null or undefined
            assertParamExists('v1AccountsAccountIdPut', 'accountId', accountId)
            const localVarPath = `/v1/accounts/{accountId}`
                .replace(`{${"accountId"}}`, encodeURIComponent(String(accountId)));
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
         * Create an account for a logged in user
         * @summary Create an expense for a logged in user
         * @param {UpsertAccount} data Update expense model
         * @param {string} authorization Bearer
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        v1AccountsPost: async (data: UpsertAccount, authorization: string, options: RawAxiosRequestConfig = {}): Promise<RequestArgs> => {
            // verify required parameter 'data' is not null or undefined
            assertParamExists('v1AccountsPost', 'data', data)
            // verify required parameter 'authorization' is not null or undefined
            assertParamExists('v1AccountsPost', 'authorization', authorization)
            const localVarPath = `/v1/accounts`;
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
        /**
         * Get accounts for the logged in user
         * @summary Get accounts for the logged in user
         * @param {string} authorization Bearer
         * @param {number} page page or 1 by default
         * @param {number} pageSize pageSize or 300 by default
         * @param {string} userId user id
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        v1UsersUserIdAccountsGet: async (authorization: string, page: number, pageSize: number, userId: string, options: RawAxiosRequestConfig = {}): Promise<RequestArgs> => {
            // verify required parameter 'authorization' is not null or undefined
            assertParamExists('v1UsersUserIdAccountsGet', 'authorization', authorization)
            // verify required parameter 'page' is not null or undefined
            assertParamExists('v1UsersUserIdAccountsGet', 'page', page)
            // verify required parameter 'pageSize' is not null or undefined
            assertParamExists('v1UsersUserIdAccountsGet', 'pageSize', pageSize)
            // verify required parameter 'userId' is not null or undefined
            assertParamExists('v1UsersUserIdAccountsGet', 'userId', userId)
            const localVarPath = `/v1/users/{userId}/accounts`
                .replace(`{${"userId"}}`, encodeURIComponent(String(userId)));
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
    }
};

/**
 * AccountsApi - functional programming interface
 * @export
 */
export const AccountsApiFp = function(configuration?: Configuration) {
    const localVarAxiosParamCreator = AccountsApiAxiosParamCreator(configuration)
    return {
        /**
         * Update an account for a logged in user
         * @summary Update an account for a logged in user
         * @param {UpsertAccount} data Update account model
         * @param {string} authorization Bearer
         * @param {string} accountId accountId
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        async v1AccountsAccountIdPut(data: UpsertAccount, authorization: string, accountId: string, options?: RawAxiosRequestConfig): Promise<(axios?: AxiosInstance, basePath?: string) => AxiosPromise<AccountResult>> {
            const localVarAxiosArgs = await localVarAxiosParamCreator.v1AccountsAccountIdPut(data, authorization, accountId, options);
            const localVarOperationServerIndex = configuration?.serverIndex ?? 0;
            const localVarOperationServerBasePath = operationServerMap['AccountsApi.v1AccountsAccountIdPut']?.[localVarOperationServerIndex]?.url;
            return (axios, basePath) => createRequestFunction(localVarAxiosArgs, globalAxios, BASE_PATH, configuration)(axios, localVarOperationServerBasePath || basePath);
        },
        /**
         * Create an account for a logged in user
         * @summary Create an expense for a logged in user
         * @param {UpsertAccount} data Update expense model
         * @param {string} authorization Bearer
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        async v1AccountsPost(data: UpsertAccount, authorization: string, options?: RawAxiosRequestConfig): Promise<(axios?: AxiosInstance, basePath?: string) => AxiosPromise<AccountResult>> {
            const localVarAxiosArgs = await localVarAxiosParamCreator.v1AccountsPost(data, authorization, options);
            const localVarOperationServerIndex = configuration?.serverIndex ?? 0;
            const localVarOperationServerBasePath = operationServerMap['AccountsApi.v1AccountsPost']?.[localVarOperationServerIndex]?.url;
            return (axios, basePath) => createRequestFunction(localVarAxiosArgs, globalAxios, BASE_PATH, configuration)(axios, localVarOperationServerBasePath || basePath);
        },
        /**
         * Get accounts for the logged in user
         * @summary Get accounts for the logged in user
         * @param {string} authorization Bearer
         * @param {number} page page or 1 by default
         * @param {number} pageSize pageSize or 300 by default
         * @param {string} userId user id
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        async v1UsersUserIdAccountsGet(authorization: string, page: number, pageSize: number, userId: string, options?: RawAxiosRequestConfig): Promise<(axios?: AxiosInstance, basePath?: string) => AxiosPromise<UserAccountsResult>> {
            const localVarAxiosArgs = await localVarAxiosParamCreator.v1UsersUserIdAccountsGet(authorization, page, pageSize, userId, options);
            const localVarOperationServerIndex = configuration?.serverIndex ?? 0;
            const localVarOperationServerBasePath = operationServerMap['AccountsApi.v1UsersUserIdAccountsGet']?.[localVarOperationServerIndex]?.url;
            return (axios, basePath) => createRequestFunction(localVarAxiosArgs, globalAxios, BASE_PATH, configuration)(axios, localVarOperationServerBasePath || basePath);
        },
    }
};

/**
 * AccountsApi - factory interface
 * @export
 */
export const AccountsApiFactory = function (configuration?: Configuration, basePath?: string, axios?: AxiosInstance) {
    const localVarFp = AccountsApiFp(configuration)
    return {
        /**
         * Update an account for a logged in user
         * @summary Update an account for a logged in user
         * @param {UpsertAccount} data Update account model
         * @param {string} authorization Bearer
         * @param {string} accountId accountId
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        v1AccountsAccountIdPut(data: UpsertAccount, authorization: string, accountId: string, options?: any): AxiosPromise<AccountResult> {
            return localVarFp.v1AccountsAccountIdPut(data, authorization, accountId, options).then((request) => request(axios, basePath));
        },
        /**
         * Create an account for a logged in user
         * @summary Create an expense for a logged in user
         * @param {UpsertAccount} data Update expense model
         * @param {string} authorization Bearer
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        v1AccountsPost(data: UpsertAccount, authorization: string, options?: any): AxiosPromise<AccountResult> {
            return localVarFp.v1AccountsPost(data, authorization, options).then((request) => request(axios, basePath));
        },
        /**
         * Get accounts for the logged in user
         * @summary Get accounts for the logged in user
         * @param {string} authorization Bearer
         * @param {number} page page or 1 by default
         * @param {number} pageSize pageSize or 300 by default
         * @param {string} userId user id
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        v1UsersUserIdAccountsGet(authorization: string, page: number, pageSize: number, userId: string, options?: any): AxiosPromise<UserAccountsResult> {
            return localVarFp.v1UsersUserIdAccountsGet(authorization, page, pageSize, userId, options).then((request) => request(axios, basePath));
        },
    };
};

/**
 * AccountsApi - object-oriented interface
 * @export
 * @class AccountsApi
 * @extends {BaseAPI}
 */
export class AccountsApi extends BaseAPI {
    /**
     * Update an account for a logged in user
     * @summary Update an account for a logged in user
     * @param {UpsertAccount} data Update account model
     * @param {string} authorization Bearer
     * @param {string} accountId accountId
     * @param {*} [options] Override http request option.
     * @throws {RequiredError}
     * @memberof AccountsApi
     */
    public v1AccountsAccountIdPut(data: UpsertAccount, authorization: string, accountId: string, options?: RawAxiosRequestConfig) {
        return AccountsApiFp(this.configuration).v1AccountsAccountIdPut(data, authorization, accountId, options).then((request) => request(this.axios, this.basePath));
    }

    /**
     * Create an account for a logged in user
     * @summary Create an expense for a logged in user
     * @param {UpsertAccount} data Update expense model
     * @param {string} authorization Bearer
     * @param {*} [options] Override http request option.
     * @throws {RequiredError}
     * @memberof AccountsApi
     */
    public v1AccountsPost(data: UpsertAccount, authorization: string, options?: RawAxiosRequestConfig) {
        return AccountsApiFp(this.configuration).v1AccountsPost(data, authorization, options).then((request) => request(this.axios, this.basePath));
    }

    /**
     * Get accounts for the logged in user
     * @summary Get accounts for the logged in user
     * @param {string} authorization Bearer
     * @param {number} page page or 1 by default
     * @param {number} pageSize pageSize or 300 by default
     * @param {string} userId user id
     * @param {*} [options] Override http request option.
     * @throws {RequiredError}
     * @memberof AccountsApi
     */
    public v1UsersUserIdAccountsGet(authorization: string, page: number, pageSize: number, userId: string, options?: RawAxiosRequestConfig) {
        return AccountsApiFp(this.configuration).v1UsersUserIdAccountsGet(authorization, page, pageSize, userId, options).then((request) => request(this.axios, this.basePath));
    }
}

