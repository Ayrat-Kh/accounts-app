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
import { ErrorResponse } from '../models';
// @ts-ignore
import { UserAccountsResult } from '../models';
// @ts-ignore
import { UserIdOrMe } from '../models';
// @ts-ignore
import { UserResult } from '../models';
/**
 * UsersApi - axios parameter creator
 * @export
 */
export const UsersApiAxiosParamCreator = function (configuration?: Configuration) {
    return {
        /**
         * Get accounts for the logged in user
         * @summary Get accounts for the logged in user
         * @param {string} authorization Bearer
         * @param {number} page page or 1 by default
         * @param {number} pageSize pageSize or 300 by default
         * @param {UserIdOrMe} userId user id
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        v1UsersUserIdAccountsGet: async (authorization: string, page: number, pageSize: number, userId: UserIdOrMe, options: RawAxiosRequestConfig = {}): Promise<RequestArgs> => {
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
        /**
         * Get user info
         * @summary Get user info
         * @param {string} authorization Bearer
         * @param {UserIdOrMe} userId uuid or me - to get info for the logged in user
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        v1UsersUserIdGet: async (authorization: string, userId: UserIdOrMe, options: RawAxiosRequestConfig = {}): Promise<RequestArgs> => {
            // verify required parameter 'authorization' is not null or undefined
            assertParamExists('v1UsersUserIdGet', 'authorization', authorization)
            // verify required parameter 'userId' is not null or undefined
            assertParamExists('v1UsersUserIdGet', 'userId', userId)
            const localVarPath = `/v1/users/{userId}`
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
 * UsersApi - functional programming interface
 * @export
 */
export const UsersApiFp = function(configuration?: Configuration) {
    const localVarAxiosParamCreator = UsersApiAxiosParamCreator(configuration)
    return {
        /**
         * Get accounts for the logged in user
         * @summary Get accounts for the logged in user
         * @param {string} authorization Bearer
         * @param {number} page page or 1 by default
         * @param {number} pageSize pageSize or 300 by default
         * @param {UserIdOrMe} userId user id
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        async v1UsersUserIdAccountsGet(authorization: string, page: number, pageSize: number, userId: UserIdOrMe, options?: RawAxiosRequestConfig): Promise<(axios?: AxiosInstance, basePath?: string) => AxiosPromise<UserAccountsResult>> {
            const localVarAxiosArgs = await localVarAxiosParamCreator.v1UsersUserIdAccountsGet(authorization, page, pageSize, userId, options);
            const localVarOperationServerIndex = configuration?.serverIndex ?? 0;
            const localVarOperationServerBasePath = operationServerMap['UsersApi.v1UsersUserIdAccountsGet']?.[localVarOperationServerIndex]?.url;
            return (axios, basePath) => createRequestFunction(localVarAxiosArgs, globalAxios, BASE_PATH, configuration)(axios, localVarOperationServerBasePath || basePath);
        },
        /**
         * Get user info
         * @summary Get user info
         * @param {string} authorization Bearer
         * @param {UserIdOrMe} userId uuid or me - to get info for the logged in user
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        async v1UsersUserIdGet(authorization: string, userId: UserIdOrMe, options?: RawAxiosRequestConfig): Promise<(axios?: AxiosInstance, basePath?: string) => AxiosPromise<UserResult>> {
            const localVarAxiosArgs = await localVarAxiosParamCreator.v1UsersUserIdGet(authorization, userId, options);
            const localVarOperationServerIndex = configuration?.serverIndex ?? 0;
            const localVarOperationServerBasePath = operationServerMap['UsersApi.v1UsersUserIdGet']?.[localVarOperationServerIndex]?.url;
            return (axios, basePath) => createRequestFunction(localVarAxiosArgs, globalAxios, BASE_PATH, configuration)(axios, localVarOperationServerBasePath || basePath);
        },
    }
};

/**
 * UsersApi - factory interface
 * @export
 */
export const UsersApiFactory = function (configuration?: Configuration, basePath?: string, axios?: AxiosInstance) {
    const localVarFp = UsersApiFp(configuration)
    return {
        /**
         * Get accounts for the logged in user
         * @summary Get accounts for the logged in user
         * @param {string} authorization Bearer
         * @param {number} page page or 1 by default
         * @param {number} pageSize pageSize or 300 by default
         * @param {UserIdOrMe} userId user id
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        v1UsersUserIdAccountsGet(authorization: string, page: number, pageSize: number, userId: UserIdOrMe, options?: any): AxiosPromise<UserAccountsResult> {
            return localVarFp.v1UsersUserIdAccountsGet(authorization, page, pageSize, userId, options).then((request) => request(axios, basePath));
        },
        /**
         * Get user info
         * @summary Get user info
         * @param {string} authorization Bearer
         * @param {UserIdOrMe} userId uuid or me - to get info for the logged in user
         * @param {*} [options] Override http request option.
         * @throws {RequiredError}
         */
        v1UsersUserIdGet(authorization: string, userId: UserIdOrMe, options?: any): AxiosPromise<UserResult> {
            return localVarFp.v1UsersUserIdGet(authorization, userId, options).then((request) => request(axios, basePath));
        },
    };
};

/**
 * UsersApi - object-oriented interface
 * @export
 * @class UsersApi
 * @extends {BaseAPI}
 */
export class UsersApi extends BaseAPI {
    /**
     * Get accounts for the logged in user
     * @summary Get accounts for the logged in user
     * @param {string} authorization Bearer
     * @param {number} page page or 1 by default
     * @param {number} pageSize pageSize or 300 by default
     * @param {UserIdOrMe} userId user id
     * @param {*} [options] Override http request option.
     * @throws {RequiredError}
     * @memberof UsersApi
     */
    public v1UsersUserIdAccountsGet(authorization: string, page: number, pageSize: number, userId: UserIdOrMe, options?: RawAxiosRequestConfig) {
        return UsersApiFp(this.configuration).v1UsersUserIdAccountsGet(authorization, page, pageSize, userId, options).then((request) => request(this.axios, this.basePath));
    }

    /**
     * Get user info
     * @summary Get user info
     * @param {string} authorization Bearer
     * @param {UserIdOrMe} userId uuid or me - to get info for the logged in user
     * @param {*} [options] Override http request option.
     * @throws {RequiredError}
     * @memberof UsersApi
     */
    public v1UsersUserIdGet(authorization: string, userId: UserIdOrMe, options?: RawAxiosRequestConfig) {
        return UsersApiFp(this.configuration).v1UsersUserIdGet(authorization, userId, options).then((request) => request(this.axios, this.basePath));
    }
}
