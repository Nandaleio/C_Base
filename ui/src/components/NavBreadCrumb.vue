<script lang="ts" setup>

import {
  Breadcrumb,
  BreadcrumbItem,
  BreadcrumbLink,
  BreadcrumbList,
  BreadcrumbPage,
  BreadcrumbSeparator,
} from '@/components/ui/breadcrumb'
import { Separator } from '@/components/ui/separator'
import { SidebarTrigger } from '@/components/ui/sidebar'
import router from '@/router';
import { ref } from 'vue';


const crumbs = ref<string[]>([])
router.afterEach((to, from, failure) => {
  crumbs.value = router.currentRoute.value.fullPath.split('/').filter(v => !!v);
})


</script>

<template>
    <div class="flex items-center gap-2 px-4">
          <SidebarTrigger class="-ml-1" />
          <Separator orientation="vertical" class="mr-2 h-4" />
          <Breadcrumb>
            <BreadcrumbList>
                <template v-for="(item, index) of crumbs">
                    <BreadcrumbItem class="hidden md:block">
                        <BreadcrumbLink href="#" class="capitalize">
                            {{ item }}
                        </BreadcrumbLink>
                    </BreadcrumbItem>
                    <BreadcrumbSeparator v-if="index < (crumbs.length-1)" class="hidden md:block" />
                </template>
            </BreadcrumbList>
          </Breadcrumb>
        </div>
</template>