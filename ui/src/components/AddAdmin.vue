<script setup lang="ts">

import {
  DialogContent,
  DialogDescription,
  DialogFooter,
  DialogHeader,
  DialogTitle,
  DialogClose
} from '@/components/ui/dialog'
import { Button } from '@/components/ui/button'
import { Input } from '@/components/ui/input';

    import { cbFetch } from '@/services/api-service';
    import { ref } from 'vue';

    const username = ref('');
    const password = ref('');
    const confirmPassword = ref('');
    
    const email = ref('');
    const avatar = ref('');

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
            password: password.value,
            email: email.value,
        });
        if(res) close();
        
    }
  
    const close = () => {error.value="";}
</script>

<template>

<DialogContent>
        <DialogHeader>
            <DialogTitle>Add new Admin</DialogTitle>
            <DialogDescription>
                Every admin created will automatically have access to the admin UI
            </DialogDescription>
        </DialogHeader>

        <Input type="file" v-model="avatar" placeholder="avatar"/>
        <Input type="text" v-model="username" placeholder="Username"/>
        <Input type="email" v-model="email" placeholder="Email"/>
        <Input type="password" v-model="password" placeholder="Password"/>
        <Input type="password" v-model="confirmPassword" placeholder="Confirm Password"/>

        
        <pre v-if="error" class="error">{{ error }}</pre>

        <DialogFooter>
            <DialogClose as-child>
                <Button @click="close" type="button" variant="secondary">
                    Cancel
                </Button>
            </DialogClose>
            <Button @click="addAdmin">Confirm</Button>
        </DialogFooter>
    </DialogContent>
</template>

<style scoped>

.error{
    color: var(--pico-form-element-invalid-active-border-color)
}
</style>