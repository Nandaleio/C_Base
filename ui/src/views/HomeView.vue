<script setup lang=ts>
import { RouterView } from 'vue-router'
import {
  Avatar,
  AvatarFallback,
  AvatarImage,
} from '@/components/ui/avatar'

import {
  DropdownMenu,
  DropdownMenuContent,
  DropdownMenuGroup,
  DropdownMenuItem,
  DropdownMenuSeparator,
  DropdownMenuTrigger,
} from '@/components/ui/dropdown-menu'
import {
  Sidebar,
  SidebarContent,
  SidebarFooter,
  SidebarHeader,
  SidebarInset,
  SidebarMenu,
  SidebarMenuButton,
  SidebarMenuItem,
  SidebarProvider,
  SidebarRail,
  SidebarGroupLabel,
  SidebarGroup,
  SidebarGroupContent
} from '@/components/ui/sidebar'
import {
  AppWindow,
  ChevronsUpDown,
  Database,
  LogOut,
  Settings,
  Pencil,
  Logs,
} from 'lucide-vue-next'
import NavBreadCrumb from '@/components/NavBreadCrumb.vue'
import {userStore} from "@/stores/user-store";
import {appStore} from "@/stores/app-store";
import router from '@/router'
import { env } from '@/utils/env'
import { onMounted } from 'vue'
import { cbFetch } from '@/services/api-service'

const list = [
        {icon: Database, path: '/tables', tooltip: "tables", title: "Tables"},
        {icon: Pencil, path: '/editor', tooltip: "editor", title: "Editor"},
        {icon: Logs, path: '/logs', tooltip: "logs", title: "Logs"}
    ]

function logout() {
      localStorage.removeItem(env.localStorageTokenKey);
      localStorage.removeItem(env.localStorageUserKey);
      router.push("/login");
}

onMounted(async () => {
    const res = await cbFetch<{version:string}>('/api/version');
    appStore.version =  res.version; 

    if(!userStore.username) {
      userStore.getLocal();
    }
})

</script>

<template>
  <SidebarProvider>
    <Sidebar collapsible="icon">
      <SidebarHeader>
        <SidebarMenu>
          <SidebarMenuItem>
            <DropdownMenu>
              <DropdownMenuTrigger as-child>
                <SidebarMenuButton size="lg"
                  class="data-[state=open]:bg-sidebar-accent data-[state=open]:text-sidebar-accent-foreground"
                >
                  <div class="flex aspect-square size-8 items-center justify-center rounded-lg bg-sidebar-primary text-sidebar-primary-foreground">
                    <AppWindow />
                  </div>
                  <div class="grid flex-1 text-left text-sm leading-tight">
                    <span class="truncate font-semibold">{{ appStore.name }}</span>
                    <span class="truncate text-xs">{{ appStore.version }}</span>
                  </div>
                </SidebarMenuButton>
              </DropdownMenuTrigger>
            </DropdownMenu>
          </SidebarMenuItem>
        </SidebarMenu>
      </SidebarHeader>

      <SidebarContent>


        <SidebarGroup>
          <SidebarGroupContent>
            <SidebarGroupLabel>Application</SidebarGroupLabel>
            <SidebarMenu>
              <SidebarMenuItem v-for="item in list" :key="item.title">
                <SidebarMenuButton as-child>
                <RouterLink :to="item.path">
                    <component :is="item.icon" />
                    <span>{{ item.title }}</span>
                </RouterLink>
              </SidebarMenuButton>
              </SidebarMenuItem>
            </SidebarMenu>
            
          </SidebarGroupContent>
        </SidebarGroup>
      </SidebarContent>

      <SidebarFooter>
        <SidebarMenu>
          <SidebarMenuItem>
            <DropdownMenu>
              <DropdownMenuTrigger as-child>
                <SidebarMenuButton
                  size="lg"
                  class="data-[state=open]:bg-sidebar-accent data-[state=open]:text-sidebar-accent-foreground"
                >
                  <Avatar class="h-8 w-8 rounded-lg">
                    <AvatarImage :src="userStore.avatar" />
                    <AvatarFallback class="rounded-lg">
                      {{ userStore.username.charAt(0).toUpperCase() }}
                    </AvatarFallback>
                  </Avatar>
                  <div class="grid flex-1 text-left text-sm leading-tight">
                    <span class="truncate font-semibold">{{ userStore.username }}</span>
                    <span class="truncate text-xs">{{ userStore.email }}</span>
                  </div>
                  <ChevronsUpDown class="ml-auto size-4" />
                </SidebarMenuButton>
              </DropdownMenuTrigger>
              <DropdownMenuContent class="w-[--radix-dropdown-menu-trigger-width] min-w-56 rounded-lg" side="bottom" align="end" :side-offset="4">
                <DropdownMenuGroup>
                  <DropdownMenuItem @click="router.push('/settings')"  class="cursor-pointer">
                    <Settings />
                    Settings
                  </DropdownMenuItem>
                </DropdownMenuGroup>
                <DropdownMenuSeparator />
                <DropdownMenuItem @click="logout" class="cursor-pointer">
                  <LogOut />
                  Log out
                </DropdownMenuItem>
              </DropdownMenuContent>
            </DropdownMenu>
          </SidebarMenuItem>
        </SidebarMenu>
      </SidebarFooter>
      <SidebarRail />
    </Sidebar>

    <SidebarInset>
      <header class="flex h-16 shrink-0 items-center gap-2 transition-[width,height] ease-linear group-has-[[data-collapsible=icon]]/sidebar-wrapper:h-12">
        <NavBreadCrumb/>
      </header>
      <div class="flex flex-1 flex-col gap-4 pt-0">
        <RouterView/>
      </div>
    </SidebarInset>
  </SidebarProvider>
</template>