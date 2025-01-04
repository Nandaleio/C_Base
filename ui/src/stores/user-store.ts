
import { reactive } from 'vue'

export const userStore = reactive({
    avatar: "",
    username: "",
    email: "",

    saveLocal() {
        localStorage.setItem(import.meta.env.VITE_LOCALSTORAGE_USER_KEY, JSON.stringify({
            avatar:this.avatar,
            username: this.username,
            email: this.email
        }))
    },

    getLocal() {
        const jsonLocal = JSON.parse(localStorage.getItem(import.meta.env.VITE_LOCALSTORAGE_USER_KEY) ?? '{}');
        this.avatar = jsonLocal.avatar;
        this.username = jsonLocal.username;
        this.email = jsonLocal.email;
    }
})