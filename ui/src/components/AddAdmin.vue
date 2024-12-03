<script setup lang="ts">
    import { cbFetch } from '@/services/api-service';
    import { ref, useTemplateRef } from 'vue';



    const username = ref('');
    const password = ref('');
    const confirmPassword = ref('');

    const error = ref('');

    async function addAdmin() {
        if(!username.value || !password.value || !confirmPassword.value) {
            error.value = "Fill all the fields";
            return;
        }

        if(password.value != confirmPassword.value) {
            error.value = "password doesn't match";
            return;
        }

        const res = await cbFetch("/api/admin/register", "POST", {
            username: username.value,
            password: password.value
        });
        if(res) close();
        
    }
  
    const dialog = useTemplateRef('dialog')

    const open = () => {if(dialog.value) dialog.value.open = true;}
    const close = () => {if(dialog.value) {error.value=""; dialog.value.open = false;}}
    defineExpose({open, close});
</script>

<template>
    <dialog ref="dialog">
        <article>
            <header>
                <h2>Add new Admin</h2>
            </header>
            <input type="text" v-model="username" placeholder="Username">
            <input type="password" v-model="password" placeholder="Password">
            <input type="password" v-model="confirmPassword" placeholder="Confirm Password">
            <pre v-if="error" class="error">{{ error }}</pre>
            <footer>
                <button class="secondary" @click="close">Cancel</button>
                <button @click="addAdmin">Confirm</button>
            </footer>
        </article>
    </dialog>
</template>

<style scoped>

.error{
    color: var(--pico-form-element-invalid-active-border-color)
}
</style>