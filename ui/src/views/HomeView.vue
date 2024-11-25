<script setup lang="ts">
    import Nav from '@/components/Nav.vue'

    const list = [
        {icon: 'database', path: '/tables'},
        {icon: 'edit', path: '/editor'},
        {icon: 'description', path: '/logs'},
        {separator: true, path:''},
        {icon: 'settings', path: '/settings'},
    ]
</script>

<template>
    <div class="main-container">
        <Nav :list=list></Nav>
        <div class="content">
            <RouterView />
        </div>
    </div>
</template>

<style scoped>

.main-container{
    display: flex;
    height: 100%;

    .content{
        flex-grow: 1;
    }
}

</style>