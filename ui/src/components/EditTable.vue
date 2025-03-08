<script setup lang="ts">
import {
    DialogContent,
    DialogDescription,
    DialogFooter,
    DialogHeader,
    DialogTitle,
    DialogClose,
    Dialog,
    DialogTrigger
} from '@/components/ui/dialog'

import {
    Tooltip,
    TooltipContent,
    TooltipProvider,
    TooltipTrigger,
} from '@/components/ui/tooltip'
import { Separator } from '@/components/ui/separator'
import { Input } from '@/components/ui/input'
import { Label } from '@/components/ui/label'
import { Tabs, TabsContent, TabsList, TabsTrigger } from '@/components/ui/tabs'
import { AlertDialogTrigger, AlertDialog } from '@/components/ui/alert-dialog'
import AlertConfirm from '@/components/AlertConfirm.vue';
import { Button } from '@/components/ui/button'
import { Trash2, Settings } from 'lucide-vue-next'
import { onMounted, ref } from 'vue';
import { cbFetch } from '@/services/api-service';

import type { Cols } from '@/utils/types';

const props = defineProps<{
    tableName: string,
    cols?: Cols[]
}>()

const open = ref();

async function deleteTable() {
    await cbFetch(`/api/admin/table/${props.tableName}`, 'DELETE');
}

const existingtables = ref<string[]>([]);
onMounted(async () => {
    const res = await cbFetch('/api/tables');
    existingtables.value = res.data.map((v: any) => { return v.name });
})

const emit = defineEmits<{
    updated: []
    deleted: []
}>()
</script>

<template>

    <Dialog :open="open" @update:open="(o) => open = o">
        <DialogTrigger>
            <TooltipProvider>
                <Tooltip>
                    <TooltipTrigger as-child>
                        <Button variant="outline">
                            <Settings />
                        </Button>
                    </TooltipTrigger>
                    <TooltipContent>
                        <p>Edit table</p>
                    </TooltipContent>
                </Tooltip>
            </TooltipProvider>
        </DialogTrigger>

        <DialogContent>
            <DialogHeader>
                <DialogTitle>Edit the table {{ tableName }}</DialogTitle>
                <DialogDescription>

                </DialogDescription>
            </DialogHeader>


            <Tabs default-value="rules">
                <TabsList>
                    <TabsTrigger value="rules">
                        Rules
                    </TabsTrigger>
                    <TabsTrigger value="columns">
                        Columns
                    </TabsTrigger>
                </TabsList>
                <TabsContent value="rules">
                    
                    <div class="grid w-full items-center gap-1.5">
                        <Label for="email">Insert rule</Label>
                        <Input type="text" />
                    </div>

                    <div class="grid w-full items-center gap-1.5">
                        <Label for="email">Delete rule</Label>
                        <Input type="text"/>
                    </div>

                    <div class="grid w-full items-center gap-1.5">
                        <Label for="email">Update rule</Label>
                        <Input type="text" />
                    </div>

                </TabsContent>
                <TabsContent value="columns">
                    
                    <div class="flex items-center" v-for="col of cols">
                        <ColumnDefinition :col="col" :existingTables="existingtables" @delete="cols?.splice(cols!.indexOf(col), 1)" />
                    </div>

                </TabsContent>
            </Tabs>

            <Separator class="my-4" label="Delete the table" />

            <Button variant="destructive">
                <AlertDialog>
                    <AlertDialogTrigger>
                            <Trash2 />
                        <AlertConfirm @confirm="emit('deleted'); open = false"
                            :description="`This action will delete the table ${tableName}`" :action="deleteTable">
                        </AlertConfirm>
                    </AlertDialogTrigger>
                </AlertDialog>
            </Button>

            <DialogFooter>
                <DialogClose as-child>
                    <Button type="button" variant="secondary">
                        Cancel
                    </Button>
                </DialogClose>
                <Button @click="">
                    Save Changes
                </Button>
            </DialogFooter>
        </DialogContent>
    </Dialog>
</template>