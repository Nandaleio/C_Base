
import { env } from '@/utils/env'
import { reactive } from 'vue'

export const userStore = reactive({
    avatar: "",
    username: "",
    email: "",

    saveLocal() {
        localStorage.setItem(env.localStorageUserKey, JSON.stringify({
            avatar:this.avatar,
            username: this.username,
            email: this.email
        }))
    },

    getLocal() {
        const jsonLocal = JSON.parse(localStorage.getItem(env.localStorageUserKey) ?? '{}');
        this.avatar = jsonLocal.avatar;
        this.username = jsonLocal.username;
        this.email = jsonLocal.email;
    }
})