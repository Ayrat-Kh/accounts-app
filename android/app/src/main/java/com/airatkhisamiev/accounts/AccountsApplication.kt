package com.airatkhisamiev.accounts

import android.app.Application

class AccountsApplication : Application() {
    lateinit var container: AppContainer
        private set

    override fun onCreate() {
        super.onCreate()
        container = AppContainer(this)
    }
}
