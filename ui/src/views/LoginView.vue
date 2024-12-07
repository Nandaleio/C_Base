
<script setup lang="ts">
import router from '@/router';
import { env } from '@/utils/env';
import { ref } from 'vue'

import { Button } from '@/components/ui/button'
import { Card, CardContent, CardDescription, CardFooter, CardHeader, CardTitle } from '@/components/ui/card'
import { Input } from '@/components/ui/input'
import { Label } from '@/components/ui/label'
import { userStore } from '@/stores/user-store';
  import { useToast } from '@/components/ui/toast/use-toast'

  const { toast } = useToast()
  const username = ref('');
  const password = ref('');

  async function login() {
    const res = await fetch(`${env.apiURL}/api/admin/login`, {
      method: 'post',
      body: JSON.stringify({username: username.value, password: password.value})
    })
    if(res.ok) {
      const loginInfo = await res.json();

      console.log(loginInfo);

      if(loginInfo.error){
        toast({
          title: 'Error while logging in',
          description: loginInfo.error,
        });
      } else {
        localStorage.setItem(env.localStorageTokenKey, loginInfo.token);
        userStore.email = loginInfo.email;
        userStore.username = loginInfo.username;
        userStore.avatar = loginInfo.avatar ?? 'https://avatar.iran.liara.run/public';
        router.push('/tables')
      }
    }
  }

</script>

<template>
  <div class="login-form">
      <Card class="w-full max-w-sm">
    <CardHeader>
      <CardTitle class="text-2xl">
        C_Base Admin
      </CardTitle>
      <CardDescription>
        Enter your email below to login to your C_Base admin UI.
      </CardDescription>
    </CardHeader>
    <CardContent class="grid gap-4">
      <div class="grid gap-2">
        <Label for="email">Email</Label>
        <Input id="email" type="email" placeholder="m@example.com" required v-model="username"/>
      </div>
      <div class="grid gap-2">
        <Label for="password">Password</Label>
        <Input id="password" type="password" required v-model="password"/>
      </div>
    </CardContent>
    <CardFooter>
      <Button class="w-full" @click="login">
        Sign in
      </Button>
    </CardFooter>
  </Card>
  </div>
</template>

<style scoped>
.login-form {
  width: 100%;
  height: 100%;

  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
}
</style>